#ifndef _TCP_SESSION_
#define _TCP_SESSION_

#include <iostream>
#include <string>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/pool/object_pool.hpp>

#include "MsgHeader.h"
#include "job_queue.h"

#include "IBusiness.h"
#include "TradeBusinessT2.h"
#include "TradeBusiness.h"
#include "TradeBusinessDingDian.h"
#include "DingDian.h"
#include "SywgConnect.h"
//#include "TCPClientSync.h"

class IMessage;



/*
session���ͷſ��Բο�
http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/example/cpp03/ssl/server.cpp

http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/example/cpp03/http/server2/connection.hpp
*/
class TcpSession : public boost::enable_shared_from_this<TcpSession>
{
public:
	typedef boost::asio::ip::tcp::socket socket_type;
	typedef boost::asio::io_service ios_type;

	typedef ios_type::strand strand_type;
	typedef job_queue<IMessage*> queue_type;

	//�ڴ�أ���ʱ����
	//typedef boost::object_pool<CustomMessage*> object_pool_type;

private:
	socket_type socket_;
	strand_type strand_;
	queue_type& queue_;

	//static object_pool_type msg_pool_;


public:
	TcpSession( ios_type& ios, queue_type& q, int msgType);
	~TcpSession();

	socket_type& socket();
	ios_type& io_service();

	virtual void start();
	virtual void close();
	


	virtual IMessage* create_request();
	virtual void read();
	virtual void handle_read_head(const boost::system::error_code& error, size_t bytes_transferred, IMessage* req);
	virtual void handle_read_msg(const boost::system::error_code& error, size_t bytes_transferred, IMessage* req);


	virtual void write(IMessage* resp);
	virtual void handle_write_head(const boost::system::error_code& error, size_t bytes_transferred, IMessage* resp);
	virtual void handle_write_msg(const boost::system::error_code& error, size_t bytes_transferred, IMessage* resp);

public:

	// �Ƿ��¼״̬
	int loginStatus;

	

	// �����̣߳�����������Ϣ������ҵ�����ӵ������Զ���ʵ�ַ��������ֻ

	// ��̨����
	TradeBusinessT2 counterT2;
	TradeBusiness counterSzkingdom;
	TradeBusinessDingDian counterApex;
	CSywgConnect counterAGC;
	//CTCPClientSync counterXinYi;

	// ��Ϣ����
	int m_msgType;

	
	// �رչ�̨����
	void CloseCounterConnect();
	// �õ���̨����
	IBusiness& GetCounterConnect(int counterType);

	/*

	 ���±�������ͨ��TcpServer��SSLServer����
	 ҵ���TradeServer��ProcessRequest�Ĳ����߼������Է��ڻỰ��sslsession������Ϊsslsession��Ψһ�ģ�ÿ��session��connectmanager, ���Լ��ٲ������ã����Ҳ����в���������
	
	ConnectManager cm;
	 */
	
};


typedef boost::shared_ptr<TcpSession> TcpSessionPtr;

#endif // _TCP_SESSION_

