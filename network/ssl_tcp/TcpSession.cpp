#include "stdafx.h"

#include <boost/bind.hpp>
#include <boost/functional/factory.hpp>



#include "TcpSession.h"

#include "FileLog.h"
#include "FileLogManager.h"
#include "DistributedLogManager.h"

#include "IMessage.h"
#include "TcpMessageOld.h"
#include "SslMessagePB.h"
#include "CustomMessage.h" 



TcpSession::TcpSession( ios_type& ios, queue_type& q, int msgType):
	socket_(ios), 
	strand_(ios), 
	queue_(q)
{
	m_msgType = msgType;
}

TcpSession::~TcpSession()
{	
}


TcpSession::socket_type& TcpSession::socket()
{
	return socket_;
}

TcpSession::ios_type& TcpSession::io_service()
{
	return socket_.get_io_service();
}

void TcpSession::close()
{
	// �رչ�̨����
//	CloseCounterConnect();

	boost::system::error_code ignored_ec;

	socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);

	socket_.close(ignored_ec);

	// �ͷ�Server������ָ��
	//delete this;
}


void TcpSession::start()
{
	read();
}

IMessage* TcpSession::create_request()
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

	req->SetTcpSession(shared_from_this());

	return req;

	// �ڴ��
	//return msg_pool_.construct(shared_from_this(), bind(&object_pool_type::destroy, ref(msg_pool_), _1));

	//tcp_session_old_ptr sess = shared_from_this();
	//tcp_old_request_ptr req = boost::factory<tcp_old_request_ptr>()(shared_from_this());
	//tcp_old_request_ptr req = new tcp_old_request((tcp_session_old_ptr)shared_from_this());
	
}


// ������
void TcpSession::read()
{
	IMessage* req = create_request();	

	boost::asio::async_read(socket_, 
		boost::asio::buffer(req->GetMsgHeader(), req->GetMsgHeaderSize()), 
		boost::asio::transfer_all(),
		strand_.wrap(
			boost::bind(&TcpSession::handle_read_head, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, req)
		)
	);
}

void TcpSession::handle_read_head(const boost::system::error_code& error, size_t bytes_transferred, IMessage* req)
{
	if (error)
	{
		gFileLog::instance().Log("TcpSession ����ͷʧ�ܻ�ͻ��˹ر����ӣ� �������:" + boost::lexical_cast<std::string>(error.value()) + ", ������Ϣ:" + error.message());

		close();
		return;
	}

	if (bytes_transferred != req->GetMsgHeaderSize())
	{
		gFileLog::instance().Log("TcpSession ����ͷʧ�ܣ���Ҫ��:" + boost::lexical_cast<std::string>(req->GetMsgHeaderSize()) + ", ʵ�ʶ�:" + boost::lexical_cast<std::string>(bytes_transferred) );

		close();
		return;
	}

	
	if (!req->DecoderMsgHeader())
	{
		gFileLog::instance().Log("TcpSession �����ͷʧ��");

		close();
		return;
	}

	boost::asio::async_read(socket_, 
		boost::asio::buffer(req->GetMsgContent(), req->GetMsgContentSize()),
		boost::asio::transfer_all(),
		strand_.wrap(
			bind(&TcpSession::handle_read_msg, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, req)
		)
	);
}



void TcpSession::handle_read_msg(const boost::system::error_code& error, size_t bytes_transferred, IMessage* req)
{
	
		
	if (error) 
	{
	
		//gFileLog::instance().Log("TcpSession ��������ʧ�ܣ� �������:" + boost::lexical_cast<std::string>(error.value()) + ", ������Ϣ:" + error.message());

		close();
		return;
	}

	if (bytes_transferred != req->GetMsgContentSize())
	{
		gFileLog::instance().Log("TcpSession ��������ʧ�� ��Ҫ��:" + boost::lexical_cast<std::string>(req->GetMsgContentSize()) + ", ʵ�ʶ�:" + boost::lexical_cast<std::string>(bytes_transferred) );

		close();
		return;
	}

	
	// ���ý��յ���ʱ��
	req->SetRecvTime();

	queue_.push(req);

	read();
}

// дӦ������
void TcpSession::write(IMessage* resp)
{
	try
	{
		if (m_msgType != resp->msgType)
		{
			AfxMessageBox("��Ϣ���ʹ���");
		}

		boost::asio::async_write(socket_,
			boost::asio::buffer(resp->GetMsgHeader(), resp->GetMsgHeaderSize()),
			boost::asio::transfer_all(),
			strand_.wrap(
				bind(&TcpSession::handle_write_head, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, resp)
			)
		);
	}
	catch(std::exception& e)
	{
		std::string expMsg = e.what();
		gFileLog::instance().Log("TcpSession write exp: " + expMsg);
	}
}

void TcpSession::handle_write_head(const boost::system::error_code& error, size_t bytes_transferred, IMessage* resp)
{
	if (error)
	{
		gFileLog::instance().Log("TcpSession д��ͷʧ�ܣ��������:" + boost::lexical_cast<std::string>(error.value()) + ", ������Ϣ:" + error.message());

		close();
		return;
	}


	if (bytes_transferred != resp->GetMsgHeaderSize())
	{
		gFileLog::instance().Log("TcpSession д��ͷʧ�� ��Ҫд:" + boost::lexical_cast<std::string>(resp->GetMsgHeaderSize()) + ", ʵ��д:" + boost::lexical_cast<std::string>(bytes_transferred) );

		close();
		return;
	}

	try
	{
		boost::asio::async_write(socket_,
			boost::asio::buffer(resp->GetMsgContent(), resp->GetMsgContentSize()),
			boost::asio::transfer_all(),
			strand_.wrap(
				bind(&TcpSession::handle_write_msg, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, resp)
			)
		);
	}
	catch(std::exception& e)
	{
		std::string expMsg = e.what();
		gFileLog::instance().Log("TcpSession handle_write_head exp: " + expMsg);
	}	
}

void TcpSession::handle_write_msg(const boost::system::error_code& error, size_t bytes_transferred, IMessage* resp)
{

	if (error)
	{

		gFileLog::instance().Log("TcpSession д������ʧ�ܣ� �������:" + boost::lexical_cast<std::string>(error.value()) + ", ������Ϣ:" + error.message());

		close();
		return;
	}

	if (bytes_transferred != resp->GetMsgContentSize())
	{
		gFileLog::instance().Log("TcpSession д������ʧ�� ��Ҫд:" + boost::lexical_cast<std::string>(resp->GetMsgContentSize()) + ", ʵ��д:" + boost::lexical_cast<std::string>(bytes_transferred) );

		close();
		return;
	}

	// ������־����
	resp->SetSendTime();

	gFileLogManager::instance().push(resp->log);
	gDistributedLogManager::instance().push(resp->log);

	/*
	if (msgHeader.FunctionNo == 0)
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



// ���ݲ��������ض�Ӧ�Ĺ�̨����
IBusiness& TcpSession::GetCounterConnect(int counterType)
{
	
	/*
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
	*/
}
