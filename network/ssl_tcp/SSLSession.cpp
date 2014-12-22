#include "stdafx.h"

#include <boost/bind.hpp>
#include <boost/functional/factory.hpp>


#include "SSLSession.h"

#include "FileLog.h"
#include "FileLogManager.h"
#include "DistributedLogManager.h"


#include "IMessage.h"
#include "TcpMessageOld.h"
#include "SslMessagePB.h"
#include "CustomMessage.h" 
#include "StatusData.h"


SSLSession::SSLSession(IOSType& ios, QueueType& q, int msgType, int port, boost::asio::ssl::context& context)
	:socket(ios, context), 
	strand(ios), 
	queue(q)
{
	//socket_.set_verify_mode(boost::asio::ssl::verify_fail_if_no_peer_cert);
	//socket_.set_verify_callback(boost::bind(&ssl_session::verify_certificate, this, _1, _2));

	this->msgType = msgType;
	this->port = port;
	logFile = "����\\network_" + boost::lexical_cast<std::string>(port);

	
}

SSLSession::~SSLSession()
{
	
	
}

SSLSession::IOSType& SSLSession::getIOService()
{
	return socket.get_io_service();
}

boost::asio::ssl::stream<boost::asio::ip::tcp::socket>::lowest_layer_type& SSLSession::getSocket()
{
	return socket.lowest_layer();
}

void SSLSession::start()
{
	if (msgType == MSG_TYPE_SSL_PB)
	{
		gStatusData.sslOldSessions++;

		if (gStatusData.sslOldSessions > gStatusData.sslOldMaxSessions)
			gStatusData.sslOldMaxSessions = gStatusData.sslOldSessions.load();
	}
	else if (msgType == MSG_TYPE_SSL_NEW)
	{
		gStatusData.sslNewSessions++;

		if (gStatusData.sslNewSessions > gStatusData.sslNewMaxSessions)
			gStatusData.sslNewMaxSessions = gStatusData.sslNewSessions.load();
	}
	else
	{
	}

	std::string clientIP = shared_from_this()->socket.lowest_layer().remote_endpoint().address().to_v4().to_string();
	int clientPort = shared_from_this()->socket.lowest_layer().remote_endpoint().port();
	client = clientIP + ":" + boost::lexical_cast<std::string>(clientPort);

		socket.async_handshake(
			boost::asio::ssl::stream_base::server,
			boost::bind(&SSLSession::OnHandShake, shared_from_this(), boost::asio::placeholders::error)
		);
}

void SSLSession::OnHandShake(const boost::system::error_code& error)
{
	if (error)
	{
		gFileLog::instance().error(shared_from_this()->logFile, shared_from_this()->client + " ����ʧ�ܣ� �������:" + boost::lexical_cast<std::string>(error.value()) + ", ������Ϣ:" + error.message());

		close();
		return;
	}	
	
	read();
}


void SSLSession::close()
{
	//CloseCounterConnect();

	boost::system::error_code ignored_ec;

	socket.lowest_layer().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);

	socket.lowest_layer().close(ignored_ec);

	if (msgType == MSG_TYPE_SSL_PB)
	{
		gStatusData.sslOldSessions--;
	}
	else if (msgType == MSG_TYPE_SSL_NEW)
	{
		gStatusData.sslNewSessions--;
	}
	else
	{
	}
}

IMessage* SSLSession::CreateRequest()
{
	IMessage* req = NULL;
	
	switch (msgType)
	{
	case MSG_TYPE_TCP_OLD:
		req = new TcpMessageOld();
		break;
	case MSG_TYPE_SSL_PB:
		req = new SslMessagePB();
		break;
	case MSG_TYPE_TCP_NEW:
		req = new CustomMessage(MSG_TYPE_TCP_NEW);
		break;
	case MSG_TYPE_SSL_NEW:
		req = new CustomMessage(MSG_TYPE_SSL_NEW);
		break;
	}
			
	req->SetSslSession(shared_from_this());

	return req;
	/*
	ssl_session_ptr sess = shared_from_this();
	ssl_request_ptr req = boost::factory<ssl_request_ptr>()(sess);
	return req;
	*/
}

// ������
void SSLSession::read()
{
	IMessage* req = CreateRequest();			

	int size = req->GetMsgHeaderSize();
	

	boost::asio::async_read(shared_from_this()->socket, 
		boost::asio::buffer(req->GetMsgHeader(), req->GetMsgHeaderSize()), 
		boost::asio::transfer_all(),
		strand.wrap(
			boost::bind(&SSLSession::OnReadHead, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, req)
		)
	);
}

void SSLSession::OnReadHead(const boost::system::error_code& error, size_t transferredBytes, IMessage* req)
{
	if (error)
	{
		if (error.value() == 2) // �ͻ��˹ر�����
		{
			gFileLog::instance().debug(shared_from_this()->logFile, shared_from_this()->client + " �ͻ��˹ر����ӣ� �������:" + boost::lexical_cast<std::string>(error.value()) + ", ������Ϣ:" + error.message());
		}
		else
		{
			gFileLog::instance().error(shared_from_this()->logFile, shared_from_this()->client + " �ͻ��˹ر����ӣ� �������:" + boost::lexical_cast<std::string>(error.value()) + ", ������Ϣ:" + error.message());
		}

		close();
		return;
	}

	if (transferredBytes != req->GetMsgHeaderSize())
	{
		gFileLog::instance().error(shared_from_this()->logFile, shared_from_this()->client + " ����ͷʧ�ܣ���Ҫ��:" + boost::lexical_cast<std::string>(req->GetMsgHeaderSize()) + ", ʵ�ʶ�:" + boost::lexical_cast<std::string>(transferredBytes) );

		close();
		return;
	}

	
	if (!req->DecoderMsgHeader())
	{
		gFileLog::instance().error(shared_from_this()->logFile, shared_from_this()->client + " �����ͷʧ��");

		close();
		return;
	}

	int size = req->GetMsgContentSize();
	

	boost::asio::async_read(shared_from_this()->socket, 
		boost::asio::buffer(req->GetMsgContent(), req->GetMsgContentSize()),
		boost::asio::transfer_all(),
		strand.wrap(
			boost::bind(&SSLSession::OnReadMsg, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, req)
		)
	);
}



void SSLSession::OnReadMsg(const boost::system::error_code& error, size_t transferredBytes, IMessage* req)
{
	
		
	if (error) 
	{
	
		gFileLog::instance().error(shared_from_this()->logFile, shared_from_this()->client + " ��������ʧ�ܣ� �������:" + boost::lexical_cast<std::string>(error.value()) + ", ������Ϣ:" + error.message());

		close();
		return;
	}

	if (transferredBytes != req->GetMsgContentSize())
	{
		gFileLog::instance().error(shared_from_this()->logFile, shared_from_this()->client + " ��������ʧ�� ��Ҫ��:" + boost::lexical_cast<std::string>(req->GetMsgContentSize()) + ", ʵ�ʶ�:" + boost::lexical_cast<std::string>(transferredBytes) );

		close();
		return;
	}

	
	// ���ý��յ���ʱ��
	req->SetRecvTime();

	queue.push(req);

	read();
}


// дӦ������
void SSLSession::write(IMessage* resp)
{
	

		boost::asio::async_write(shared_from_this()->socket,
			boost::asio::buffer(resp->GetMsgHeader(), resp->GetMsgHeaderSize()),
			boost::asio::transfer_all(),
			strand.wrap(
				boost::bind(&SSLSession::OnWriteHead, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, resp)
			)
		);
	
}

void SSLSession::OnWriteHead(const boost::system::error_code& error, size_t transferredBytes, IMessage* resp)
{
	if (error)
	{
		gFileLog::instance().error(shared_from_this()->logFile, shared_from_this()->client + " д��ͷʧ�ܣ��������:" + boost::lexical_cast<std::string>(error.value()) + ", ������Ϣ:" + error.message());

		close();
		return;
	}


	if (transferredBytes != resp->GetMsgHeaderSize())
	{
		gFileLog::instance().error(shared_from_this()->logFile, shared_from_this()->client + " д��ͷʧ�� ��Ҫд:" + boost::lexical_cast<std::string>(resp->GetMsgHeaderSize()) + ", ʵ��д:" + boost::lexical_cast<std::string>(transferredBytes) );

		close();
		return;
	}

	
		boost::asio::async_write(shared_from_this()->socket,
			boost::asio::buffer(resp->GetMsgContent(), resp->GetMsgContentSize()),
			boost::asio::transfer_all(),
			strand.wrap(
				boost::bind(&SSLSession::OnWriteMsg, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, resp)
			)
		);
	
	
}

void SSLSession::OnWriteMsg(const boost::system::error_code& error, size_t transferredBytes, IMessage* resp)
{

	if (error)
	{

		gFileLog::instance().error(shared_from_this()->logFile, shared_from_this()->client + " д������ʧ�ܣ� �������:" + boost::lexical_cast<std::string>(error.value()) + ", ������Ϣ:" + error.message());

		close();
		return;
	}

	if (transferredBytes != resp->GetMsgContentSize())
	{
		gFileLog::instance().error(shared_from_this()->logFile, shared_from_this()->client + " д������ʧ�� ��Ҫд:" + boost::lexical_cast<std::string>(resp->GetMsgContentSize()) + ", ʵ��д:" + boost::lexical_cast<std::string>(error) );

		close();
		return;
	}

	// ������־����
	resp->SetSendTime();

	// д��־
	gFileLogManager::instance().push(resp->log);
	//gDistributedLogManager::instance().push(resp->log);

	/*
	if (resp->GetMsgHeader()->FunctionNo == 0)
	{
		// �������ܲ�д��־
	}
	else
	{
		
	}
	*/

	// ɾ��Ӧ���
	resp->destroy();
}
