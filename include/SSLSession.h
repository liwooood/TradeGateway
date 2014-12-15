#ifndef _SSL_SESSION_H_
#define _SSL_SESSION_H_

#include <iostream>
#include <string>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/pool/object_pool.hpp>
#include <boost/asio/ssl.hpp>


#include "QueueThreadSafe.h"

#include "IBusiness.h"
#include "TradeBusinessHS.h"
#include "TradeBusinessJZ.h"
#include "TradeBusinessDD.h"
#include "DingDian.h"
#include "TradeBusinessJSD.h"
//#include "TCPClientSync.h"



#include "ISession.h"
class IMessage;

class SSLSession : public boost::enable_shared_from_this<SSLSession>, public ISession
{
public:
	
	typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> SocketType;
	typedef boost::asio::io_service IOSType;

	typedef IOSType::strand StrandType;
	typedef QueueThreadSafe<IMessage*> QueueType;
	

private:
	SocketType socket;
	StrandType strand;
	QueueType& queue;
	//static object_pool_type msg_pool_;
	

	// 消息类型
	int msgType;

public:
	SSLSession(IOSType& ios, QueueType& q, int msgType, boost::asio::ssl::context& context);
	~SSLSession();

	SocketType::lowest_layer_type& getSocket();
	IOSType& getIOService();

	virtual void start();
	virtual void close();

	
	void OnHandShake(const boost::system::error_code& error);

	virtual IMessage* CreateRequest();
	virtual void read();
	virtual void OnReadHead(const boost::system::error_code& error, size_t transferredBytes, IMessage* req);
	virtual void OnReadMsg(const boost::system::error_code& error, size_t transferredBytes, IMessage* req);
	
	virtual void write(IMessage* resp);
	virtual void OnWriteHead(const boost::system::error_code& error, size_t transferredBytes, IMessage* resp);
	virtual void OnWriteMsg(const boost::system::error_code& error, size_t transferredBytes, IMessage* resp);
	

	


};

typedef boost::shared_ptr<SSLSession> SSLSessionPtr;

#endif // _TCP_SESSION_

