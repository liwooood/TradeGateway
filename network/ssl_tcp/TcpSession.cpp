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
#include "StatusData.h"


TcpSession::MemPoolType TcpSession::memPool;

TcpSession::TcpSession(IOSType& ios, QueueType& q, int msgType, int port):
	socket(ios), 
	strand(ios), 
	queue(q)
{
	this->msgType = msgType;
	this->port = port;
	logFile = "网络\\network_" + boost::lexical_cast<std::string>(port);
	
}

TcpSession::~TcpSession()
{	
}


TcpSession::SocketType& TcpSession::getSocket()
{
	return socket;
}

TcpSession::IOSType& TcpSession::getIOService()
{
	return socket.get_io_service();
}


void TcpSession::start()
{
	if (msgType == MSG_TYPE_TCP_OLD)
	{
		gStatusData.tcpOldSessions++;

		if (gStatusData.tcpOldSessions > gStatusData.tcpOldMaxSessions)
			gStatusData.tcpOldMaxSessions = gStatusData.tcpOldSessions.load();
	}
	else if (msgType == MSG_TYPE_TCP_NEW)
	{
		gStatusData.tcpNewSessions++;

		if (gStatusData.tcpNewSessions > gStatusData.tcpNewMaxSessions)
			gStatusData.tcpNewMaxSessions = gStatusData.tcpNewSessions.load();
	}
	else
	{
	}

	std::string clientIP = shared_from_this()->socket.remote_endpoint().address().to_v4().to_string();
	int clientPort = shared_from_this()->socket.remote_endpoint().port();
	client = clientIP + ":" + boost::lexical_cast<std::string>(clientPort);

	read();
}

void TcpSession::close()
{
	// 关闭柜台连接
	//CloseCounterConnect();

	boost::system::error_code ignored_ec;

	socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);

	socket.close(ignored_ec);

	if (msgType == MSG_TYPE_TCP_OLD)
	{
		gStatusData.tcpOldSessions--;
	}
	else if (msgType == MSG_TYPE_TCP_NEW)
	{
		gStatusData.tcpNewSessions--;
	}
	else
	{
	}
}

IMessage* TcpSession::CreateRequest()
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

	req->SetTcpSession(shared_from_this());

	return req;

	// 内存池
	//return msg_pool_.construct(shared_from_this(), bind(&object_pool_type::destroy, ref(msg_pool_), _1));

	//tcp_session_old_ptr sess = shared_from_this();
	//tcp_old_request_ptr req = boost::factory<tcp_old_request_ptr>()(shared_from_this());
	//tcp_old_request_ptr req = new tcp_old_request((tcp_session_old_ptr)shared_from_this());
	
}


// 读请求
void TcpSession::read()
{
	


	IMessage* req = CreateRequest();	

	boost::asio::async_read(shared_from_this()->socket, 
		boost::asio::buffer(req->GetMsgHeader(), req->GetMsgHeaderSize()), 
		boost::asio::transfer_all(),
		strand.wrap(
			boost::bind(&TcpSession::OnReadHead, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, req)
		)
	);
}

void TcpSession::OnReadHead(const boost::system::error_code& error, size_t transferredBytes, IMessage* req)
{
	
	if (error)
	{
		if (error.value() == 2)
		{
			gFileLog::instance().debug(shared_from_this()->logFile, shared_from_this()->client + " 客户端关闭连接， 错误代码:" + boost::lexical_cast<std::string>(error.value()) + ", 错误消息:" + error.message());
		}
		else
		{
			gFileLog::instance().error(shared_from_this()->logFile, shared_from_this()->client + " 客户端关闭连接， 错误代码:" + boost::lexical_cast<std::string>(error.value()) + ", 错误消息:" + error.message());
		}

		close();
		return;
	}

	if (transferredBytes != req->GetMsgHeaderSize())
	{
		gFileLog::instance().error(shared_from_this()->logFile, shared_from_this()->client + " 读包头失败，需要读:" + boost::lexical_cast<std::string>(req->GetMsgHeaderSize()) + ", 实际读:" + boost::lexical_cast<std::string>(transferredBytes) );

		close();
		return;
	}

	
	if (!req->DecoderMsgHeader())
	{
		gFileLog::instance().error(shared_from_this()->logFile, shared_from_this()->client + " 解码包头失败");

		close();
		return;
	}

	boost::asio::async_read(shared_from_this()->socket, 
		boost::asio::buffer(req->GetMsgContent(), req->GetMsgContentSize()),
		boost::asio::transfer_all(),
		strand.wrap(
			boost::bind(&TcpSession::OnReadMsg, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, req)
		)
	);
}



void TcpSession::OnReadMsg(const boost::system::error_code& error, size_t transferredBytes, IMessage* req)
{
	
		
	if (error) 
	{
	
		gFileLog::instance().error(shared_from_this()->logFile, shared_from_this()->client + " 读包内容失败， 错误代码:" + boost::lexical_cast<std::string>(error.value()) + ", 错误消息:" + error.message());

		close();
		return;
	}

	if (transferredBytes != req->GetMsgContentSize())
	{
		gFileLog::instance().error(shared_from_this()->logFile, shared_from_this()->client + " 读包内容失败 需要读:" + boost::lexical_cast<std::string>(req->GetMsgContentSize()) + ", 实际读:" + boost::lexical_cast<std::string>(transferredBytes) );

		close();
		return;
	}

	
	// 设置接收到的时间
	req->SetRecvTime();

	queue.push(req);

	read();
}

// 写应答数据
void TcpSession::write(IMessage* resp)
{
		boost::asio::async_write(shared_from_this()->socket,
			boost::asio::buffer(resp->GetMsgHeader(), resp->GetMsgHeaderSize()),
			boost::asio::transfer_all(),
			strand.wrap(
				boost::bind(&TcpSession::OnWriteHead, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, resp)
			)
		);
	
}

void TcpSession::OnWriteHead(const boost::system::error_code& error, size_t transferredBytes, IMessage* resp)
{
	if (error)
	{
		gFileLog::instance().error(shared_from_this()->logFile, shared_from_this()->client + " 写包头失败，错误代码:" + boost::lexical_cast<std::string>(error.value()) + ", 错误消息:" + error.message());

		close();
		return;
	}


	if (transferredBytes != resp->GetMsgHeaderSize())
	{
		gFileLog::instance().error(shared_from_this()->logFile, shared_from_this()->client + " 写包头失败 需要写:" + boost::lexical_cast<std::string>(resp->GetMsgHeaderSize()) + ", 实际写:" + boost::lexical_cast<std::string>(transferredBytes) );

		close();
		return;
	}

	
		boost::asio::async_write(shared_from_this()->socket,
			boost::asio::buffer(resp->GetMsgContent(), resp->GetMsgContentSize()),
			boost::asio::transfer_all(),
			strand.wrap(
				boost::bind(&TcpSession::OnWriteMsg, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, resp)
			)
		);
	
}

void TcpSession::OnWriteMsg(const boost::system::error_code& error, size_t transferredBytes, IMessage* resp)
{

	if (error)
	{

		gFileLog::instance().error(shared_from_this()->logFile, shared_from_this()->client + " 写包内容失败， 错误代码:" + boost::lexical_cast<std::string>(error.value()) + ", 错误消息:" + error.message());

		close();
		return;
	}

	if (transferredBytes != resp->GetMsgContentSize())
	{
		gFileLog::instance().error(shared_from_this()->logFile, shared_from_this()->client + " 写包内容失败 需要写:" + boost::lexical_cast<std::string>(resp->GetMsgContentSize()) + ", 实际写:" + boost::lexical_cast<std::string>(transferredBytes) );

		close();
		return;
	}

	// 存入日志队列
	resp->SetSendTime();
	gFileLogManager::instance().push(resp->log);
	//gDistributedLogManager::instance().push(resp->log);


	// 删除应答包
	resp->destroy();
}

