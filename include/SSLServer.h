#ifndef SSL_SERVER_H
#define SSL_SERVER_H


#include <iostream>
#include <string>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/functional/factory.hpp>
#include <boost/asio/ssl.hpp>

#include "SSLSession.h"
#include "IOServicePool.h"

#include "ConfigManager.h"

class SSLServer
{
public:
	typedef SSLSession::IOSType IOSType;
	typedef SSLSession::QueueType QueueType;

	typedef boost::asio::ip::tcp::acceptor AcceptorType;
	typedef boost::asio::ip::tcp TCPType;

private:
	IOServicePool& iosPool;
	QueueType& queue;

	AcceptorType acceptor;
	
	boost::asio::ssl::context context;

	int msgType;
	//SSLSessionPtr m_session;
	int port;
	std::string logFile;
	
public:
	SSLServer( unsigned short port, QueueType& q, int msgType, int n=4);
	SSLServer(IOServicePool& ios, unsigned short port, QueueType& q, int msgType);
	void start();
	void run();
	void stop();

	//bool verify_certificate(bool preverified,      boost::asio::ssl::verify_context& ctx);
	//std::string get_password();
	
	

private:
	void StartAccept();
	void OnAccept(const boost::system::error_code& error, SSLSessionPtr session);
	
	
};

#endif
