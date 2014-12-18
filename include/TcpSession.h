#ifndef TCP_SESSION_H
#define TCP_SESSION_H

#include <iostream>
#include <string>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/pool/object_pool.hpp>


#include "QueueThreadSafe.h"

#include "IBusiness.h"
#include "TradeBusinessHS.h"
#include "TradeBusinessJZ.h"
#include "TradeBusinessDD.h"
#include "TradeBusinessJSD.h"
//#include "TCPClientSync.h"

#include "ISession.h"

class IMessage;



/*
session的释放可以参考
http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/example/cpp03/ssl/server.cpp

http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/example/cpp03/http/server2/connection.hpp
*/
class TcpSession : public boost::enable_shared_from_this<TcpSession>, public ISession
{
public:
	typedef boost::asio::ip::tcp::socket SocketType;
	typedef boost::asio::io_service IOSType;

	typedef IOSType::strand StrandType;
	typedef QueueThreadSafe<IMessage*> QueueType;

	//内存池，暂时不用
	typedef boost::object_pool<IMessage*> MemPoolType;

private:
	SocketType socket;
	StrandType strand;
	QueueType& queue;

	static MemPoolType memPool;
	
	int msgType;
	int port;

	std::string logFile;
	std::string client;


public:
	TcpSession(IOSType& ios, QueueType& q, int msgType, int port);
	~TcpSession();

	SocketType& getSocket();
	IOSType& getIOService();



	virtual void start();
	virtual void close();
	

	virtual IMessage* CreateRequest();
	virtual void read();
	virtual void OnReadHead(const boost::system::error_code& error, size_t transferredBytes, IMessage* req);
	virtual void OnReadMsg(const boost::system::error_code& error, size_t transferredBytes, IMessage* req);


	virtual void write(IMessage* resp);
	virtual void OnWriteHead(const boost::system::error_code& error, size_t transferredBytes, IMessage* resp);
	virtual void OnWriteMsg(const boost::system::error_code& error, size_t transferredBytes, IMessage* resp);	
};


typedef boost::shared_ptr<TcpSession> TcpSessionPtr;

#endif // _TCP_SESSION_

