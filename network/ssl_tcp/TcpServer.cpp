#include "stdafx.h"

#include "TcpServer.h"

#include "FileLog.h"



TcpServer::TcpServer(unsigned short port, QueueType& q, int msgType, int n):
	  iosPool(*boost::factory<IOServicePool*>()(n)),
	  queue(q),
	  acceptor(iosPool.get(), TCPType::endpoint(TCPType::v4(), port))
	  //,session()
{
	this->msgType = msgType;
	
	acceptor.set_option(AcceptorType::reuse_address(true));
	// ��������ѡ��

	StartAccept();
}


TcpServer::TcpServer(IOServicePool& ios, unsigned short port, QueueType& q, int msgType):
	iosPool(ios),
	queue(q),
	acceptor(iosPool.get(), TCPType::endpoint(TCPType::v4(), port))
	//,m_session()
{
	this->msgType = msgType;

	acceptor.set_option(AcceptorType::reuse_address(true));

	StartAccept();
}

void TcpServer::StartAccept()
{
	//session.reset(new TcpSession(ios_pool_.get(), queue_, m_msgType));
	TcpSessionPtr session = boost::factory<TcpSessionPtr>()(iosPool.get(), queue, msgType);

		acceptor.async_accept( session->socket(), 
			boost::bind(&TcpServer::OnAccept, 
			this, 
			boost::asio::placeholders::error, 
			session));

}
// sess��Ҫ��session��ͻ
void TcpServer::OnAccept(const boost::system::error_code& error, TcpSessionPtr session)
{
	StartAccept();

	if (error)
	{
		gFileLog::instance().error("network", "TcpServer OnAccept���������:" + boost::lexical_cast<std::string>(error.value()) + "��������Ϣ:" + error.message());

		session->close();

		// �ͷ���Դ
		//delete session;
			
	}
	else
	{
		session->start();
	}

	
}


void TcpServer::start()
{
	iosPool.start();
}

void TcpServer::run()
{
	iosPool.run();
}

void TcpServer::stop()
{
	iosPool.stop();
}

