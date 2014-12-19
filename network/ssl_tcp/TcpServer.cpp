#include "stdafx.h"

#include "TcpServer.h"

#include "FileLog.h"



TcpServer::TcpServer(unsigned short port, QueueType& q, int msgType, int ioThreadNum):
	  iosPool(*boost::factory<IOServicePool*>()(ioThreadNum)),
	  queue(q),
	  acceptor(iosPool.get(), TCPType::endpoint(TCPType::v4(), port))
	  //,session()
{
	this->msgType = msgType;
	this->port = port;
	logFile = "network_" + boost::lexical_cast<std::string>(port);
	
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
	this->port = port;
	logFile = "����\\network_" + boost::lexical_cast<std::string>(port);

	acceptor.set_option(AcceptorType::reuse_address(true));

	StartAccept();
}

void TcpServer::StartAccept()
{
	//session.reset(new TcpSession(ios_pool_.get(), queue_, m_msgType));
	TcpSessionPtr session = boost::factory<TcpSessionPtr>()(iosPool.get(), queue, msgType, port);

		acceptor.async_accept(session->getSocket(), 
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
		gFileLog::instance().error(logFile, "TcpServer OnAccept���������:" + boost::lexical_cast<std::string>(error.value()) + "��������Ϣ:" + error.message());

		session->close();

		// �ͷ���Դ
		//delete session;
			
	}
	else
	{
		//session->getSocket().remote_endpoint();
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

