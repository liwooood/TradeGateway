#ifndef TCP_SERVER_H
#define TCP_SERVER_H



#include <iostream>
#include <string>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/functional/factory.hpp>

#include "TcpSession.h"
#include "IOServicePool.h"


/*
session的释放可以参考
http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/example/cpp03/ssl/server.cpp

http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/example/cpp03/http/server2/connection.hpp
*/
class TcpServer
{
public:
	typedef boost::asio::ip::tcp::acceptor AcceptorType;
	typedef boost::asio::ip::tcp TCPType;

	typedef TcpSession::ios_type IOSType;
	typedef TcpSession::queue_type QueueType;

private:
	IOServicePool& iosPool;
	QueueType& queue;

	AcceptorType acceptor;
	
	int msgType;
	//TcpSessionPtr session;


public:
	TcpServer(unsigned short port, QueueType& q, int msgType, int n=4);
	TcpServer(IOServicePool& IOSPool, unsigned short port, QueueType& q, int msgType);
	
	

	


private:
	void StartAccept();
	void OnAccept(const boost::system::error_code& error, TcpSessionPtr session);
	

public:
	void start();
	void run();
	void stop();
	
	
};

#endif
