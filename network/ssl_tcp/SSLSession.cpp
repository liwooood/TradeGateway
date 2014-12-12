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



SSLSession::SSLSession(ios_type& ios, queue_type& q, int msgType, boost::asio::ssl::context& context)
	:socket_(ios, context), 
	strand_(ios), 
	queue_(q)
{
	//socket_.set_verify_mode(boost::asio::ssl::verify_fail_if_no_peer_cert);
	//socket_.set_verify_callback(boost::bind(&ssl_session::verify_certificate, this, _1, _2));

	m_msgType = msgType;

	
}

SSLSession::~SSLSession()
{
	TRACE("~SSLSession()\n");
	
}

SSLSession::ios_type& SSLSession::io_service()
{
	return socket_.get_io_service();
}

boost::asio::ssl::stream<boost::asio::ip::tcp::socket>::lowest_layer_type& SSLSession::socket()
{
	return socket_.lowest_layer();
}

void SSLSession::close()
{
	CloseCounterConnect();

	boost::system::error_code ignored_ec;

	socket_.lowest_layer().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);

	socket_.lowest_layer().close(ignored_ec);

	// �ͷ�SSLServer������ָ��
	//delete this;
}


void SSLSession::start()
{
	try
	{
		socket_.async_handshake(
			boost::asio::ssl::stream_base::server,
			boost::bind(&SSLSession::handle_handshake, shared_from_this(), boost::asio::placeholders::error)
		);
	}
	catch(std::exception& e)
	{
		e.what();
	}
  
}

void SSLSession::handle_handshake(const boost::system::error_code& error)
{
	if (error)
	{
		gFileLog::instance().Log("SSLSession ����ʧ�ܣ� �������:" + boost::lexical_cast<std::string>(error.value()) + ", ������Ϣ:" + error.message());

		close();
		return;
	}	
	
	read();
}

IMessage* SSLSession::create_request()
{
	IMessage* req = NULL;
	
	switch (m_msgType)
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
	IMessage* req = create_request();			

	int size = req->GetMsgHeaderSize();
	std::string msg = "��Ҫ���յİ�ͷ�ֽڴ�С" + boost::lexical_cast<std::string>(size);
	gFileLog::instance().Log(msg, 0, "temp.txt");

	boost::asio::async_read(socket_, 
		boost::asio::buffer(req->GetMsgHeader(), req->GetMsgHeaderSize()), 
		boost::asio::transfer_all(),
		strand_.wrap(
			boost::bind(&SSLSession::handle_read_head, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, req)
		)
	);
}

void SSLSession::handle_read_head(const boost::system::error_code& error, size_t bytes_transferred, IMessage* req)
{
	if (error)
	{
		gFileLog::instance().Log("SSLSession ����ͷʧ�ܻ�ͻ��˹ر����ӣ� �������:" + boost::lexical_cast<std::string>(error.value()) + ", ������Ϣ:" + error.message());

		close();
		return;
	}

	if (bytes_transferred != req->GetMsgHeaderSize())
	{
		gFileLog::instance().Log("SSLSession ����ͷʧ�ܣ���Ҫ��:" + boost::lexical_cast<std::string>(req->GetMsgHeaderSize()) + ", ʵ�ʶ�:" + boost::lexical_cast<std::string>(bytes_transferred) );

		close();
		return;
	}

	
	if (!req->DecoderMsgHeader())
	{
		gFileLog::instance().Log("SSLSession �����ͷʧ��");

		close();
		return;
	}

	int size = req->GetMsgContentSize();
	std::string msg = "��Ҫ���յİ����ֽڴ�С" + boost::lexical_cast<std::string>(size);
	gFileLog::instance().Log(msg, 0, "temp.txt");

	boost::asio::async_read(socket_, 
		boost::asio::buffer(req->GetMsgContent(), req->GetMsgContentSize()),
		boost::asio::transfer_all(),
		strand_.wrap(
			bind(&SSLSession::handle_read_msg, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, req)
		)
	);
}



void SSLSession::handle_read_msg(const boost::system::error_code& error, size_t bytes_transferred, IMessage* req)
{
	
		
	if (error) 
	{
	
		gFileLog::instance().Log("SSLSession ��������ʧ�ܣ� �������:" + boost::lexical_cast<std::string>(error.value()) + ", ������Ϣ:" + error.message());

		close();
		return;
	}

	if (bytes_transferred != req->GetMsgContentSize())
	{
		gFileLog::instance().Log("SSLSession ��������ʧ�� ��Ҫ��:" + boost::lexical_cast<std::string>(req->GetMsgContentSize()) + ", ʵ�ʶ�:" + boost::lexical_cast<std::string>(bytes_transferred) );

		close();
		return;
	}

	
	// ���ý��յ���ʱ��
	req->SetRecvTime();

	queue_.push(req);

	read();
}


// дӦ������
void SSLSession::write(IMessage* resp)
{
	try
	{
		if (m_msgType != resp->msgType)
		{
			AfxMessageBox("��Ϣ���ʹ���");
		}

		int size = resp->GetMsgHeaderSize();
	std::string msg = "��Ҫ���͵İ�ͷ�ֽڴ�С" + boost::lexical_cast<std::string>(size);
	gFileLog::instance().Log(msg, 0, "temp.txt");

		boost::asio::async_write(socket_,
			boost::asio::buffer(resp->GetMsgHeader(), resp->GetMsgHeaderSize()),
			boost::asio::transfer_all(),
			strand_.wrap(
				bind(&SSLSession::handle_write_head, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, resp)
			)
		);
	}
	catch(std::exception& e)
	{
		std::string expMsg = e.what();
		gFileLog::instance().Log("SSLSession write exp: " + expMsg);
	}
}

void SSLSession::handle_write_head(const boost::system::error_code& error, size_t bytes_transferred, IMessage* resp)
{
	if (error)
	{
		gFileLog::instance().Log("SSLSession д��ͷʧ�ܣ��������:" + boost::lexical_cast<std::string>(error.value()) + ", ������Ϣ:" + error.message());

		close();
		return;
	}


	if (bytes_transferred != resp->GetMsgHeaderSize())
	{
		gFileLog::instance().Log("SSLSession д��ͷʧ�� ��Ҫд:" + boost::lexical_cast<std::string>(resp->GetMsgHeaderSize()) + ", ʵ��д:" + boost::lexical_cast<std::string>(bytes_transferred) );

		close();
		return;
	}

	try
	{
		int size = resp->GetMsgContentSize();
	std::string msg = "��Ҫ���͵İ����ֽڴ�С" + boost::lexical_cast<std::string>(size);
	gFileLog::instance().Log(msg, 0, "temp.txt");

		boost::asio::async_write(socket_,
			boost::asio::buffer(resp->GetMsgContent(), resp->GetMsgContentSize()),
			boost::asio::transfer_all(),
			strand_.wrap(
				bind(&SSLSession::handle_write_msg, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, resp)
			)
		);
	}
	catch(std::exception& e)
	{
		std::string expMsg = e.what();
		gFileLog::instance().Log("SSLSession handle_write_head exp: " + expMsg);
	}
	
}

void SSLSession::handle_write_msg(const boost::system::error_code& error, size_t bytes_transferred, IMessage* resp)
{

	if (error)
	{

		gFileLog::instance().Log("SSLSession д������ʧ�ܣ� �������:" + boost::lexical_cast<std::string>(error.value()) + ", ������Ϣ:" + error.message());

		close();
		return;
	}

	if (bytes_transferred != resp->GetMsgContentSize())
	{
		gFileLog::instance().Log("SSLSession д������ʧ�� ��Ҫд:" + boost::lexical_cast<std::string>(resp->GetMsgContentSize()) + ", ʵ��д:" + boost::lexical_cast<std::string>(bytes_transferred) );

		close();
		return;
	}

	// ������־����
	resp->SetSendTime();

	// д��־
	gFileLogManager::instance().push(resp->log);
	gDistributedLogManager::instance().push(resp->log);

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

void SSLSession::CloseCounterConnect()
{
	
	
		counterT2.CloseConnect();
		
	
		counterSzkingdom.CloseConnect();
		
	
		counterApex.CloseConnect();
		
	
		counterAGC.CloseConnect();
	
		//counterXinYi.CloseConnect();
	
}

// ���ݲ��������ض�Ӧ�Ĺ�̨����
IBusiness& SSLSession::GetCounterConnect(int counterType)
{

	

	if(counterType == COUNTER_TYPE_HS_T2)
	{
	
		return counterT2;
	}
	
	else if(counterType == COUNTER_TYPE_JZ_WIN)
	{
	
		return counterSzkingdom;
	}
	
	else if(counterType == COUNTER_TYPE_DINGDIAN)
	{
		return counterApex;
	}
	else if(counterType == COUNTER_TYPE_JSD)
	{
		return counterAGC;
	}
	else if(counterType == COUNTER_TYPE_XINYI)
	{
	
	}
	else
	{
	}	
}
