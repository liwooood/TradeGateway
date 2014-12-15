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



SSLSession::SSLSession(IOSType& ios, QueueType& q, int msgType, boost::asio::ssl::context& context)
	:socket(ios, context), 
	strand(ios), 
	queue(q)
{
	//socket_.set_verify_mode(boost::asio::ssl::verify_fail_if_no_peer_cert);
	//socket_.set_verify_callback(boost::bind(&ssl_session::verify_certificate, this, _1, _2));

	this->msgType = msgType;

	
}

SSLSession::~SSLSession()
{
	
	
}

SSLSession::IOSType& SSLSession::getIOService()
{
	return socket.get_io_service();
}

boost::asio::ssl::stream<boost::asio::ip::tcp::socket>::lowest_layer_type& SSLSession::getSocket()
{
	return socket.lowest_layer();
}

void SSLSession::close()
{
	CloseCounterConnect();

	boost::system::error_code ignored_ec;

	socket.lowest_layer().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);

	socket.lowest_layer().close(ignored_ec);
}


void SSLSession::start()
{
		socket.async_handshake(
			boost::asio::ssl::stream_base::server,
			boost::bind(&SSLSession::OnHandShake, shared_from_this(), boost::asio::placeholders::error)
		);
}

void SSLSession::OnHandShake(const boost::system::error_code& error)
{
	if (error)
	{
		gFileLog::instance().error("network", "SSLSession 握手失败， 错误代码:" + boost::lexical_cast<std::string>(error.value()) + ", 错误消息:" + error.message());

		close();
		return;
	}	
	
	read();
}

IMessage* SSLSession::CreateRequest()
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
			
	req->SetSslSession(shared_from_this());

	return req;
	/*
	ssl_session_ptr sess = shared_from_this();
	ssl_request_ptr req = boost::factory<ssl_request_ptr>()(sess);
	return req;
	*/
}

// 读请求
void SSLSession::read()
{
	IMessage* req = CreateRequest();			

	int size = req->GetMsgHeaderSize();
	

	boost::asio::async_read(socket, 
		boost::asio::buffer(req->GetMsgHeader(), req->GetMsgHeaderSize()), 
		boost::asio::transfer_all(),
		strand.wrap(
			boost::bind(&SSLSession::OnReadHead, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, req)
		)
	);
}

void SSLSession::OnReadHead(const boost::system::error_code& error, size_t transferredBytes, IMessage* req)
{
	if (error)
	{
		gFileLog::instance().error("network", "SSLSession 读包头失败或客户端关闭连接， 错误代码:" + boost::lexical_cast<std::string>(error.value()) + ", 错误消息:" + error.message());

		close();
		return;
	}

	if (transferredBytes != req->GetMsgHeaderSize())
	{
		gFileLog::instance().Log("SSLSession 读包头失败，需要读:" + boost::lexical_cast<std::string>(req->GetMsgHeaderSize()) + ", 实际读:" + boost::lexical_cast<std::string>(transferredBytes) );

		close();
		return;
	}

	
	if (!req->DecoderMsgHeader())
	{
		gFileLog::instance().error("network", "SSLSession 解码包头失败");

		close();
		return;
	}

	int size = req->GetMsgContentSize();
	

	boost::asio::async_read(socket, 
		boost::asio::buffer(req->GetMsgContent(), req->GetMsgContentSize()),
		boost::asio::transfer_all(),
		strand.wrap(
			boost::bind(&SSLSession::OnReadMsg, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, req)
		)
	);
}



void SSLSession::OnReadMsg(const boost::system::error_code& error, size_t transferredBytes, IMessage* req)
{
	
		
	if (error) 
	{
	
		gFileLog::instance().error("network_ssl", "SSLSession 读包内容失败， 错误代码:" + boost::lexical_cast<std::string>(error.value()) + ", 错误消息:" + error.message());

		close();
		return;
	}

	if (transferredBytes != req->GetMsgContentSize())
	{
		gFileLog::instance().error("network", "SSLSession 读包内容失败 需要读:" + boost::lexical_cast<std::string>(req->GetMsgContentSize()) + ", 实际读:" + boost::lexical_cast<std::string>(transferredBytes) );

		close();
		return;
	}

	
	// 设置接收到的时间
	req->SetRecvTime();

	queue.push(req);

	read();
}


// 写应答数据
void SSLSession::write(IMessage* resp)
{
	

		boost::asio::async_write(socket,
			boost::asio::buffer(resp->GetMsgHeader(), resp->GetMsgHeaderSize()),
			boost::asio::transfer_all(),
			strand.wrap(
				boost::bind(&SSLSession::OnWriteHead, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, resp)
			)
		);
	
}

void SSLSession::OnWriteHead(const boost::system::error_code& error, size_t transferredBytes, IMessage* resp)
{
	if (error)
	{
		gFileLog::instance().error("network", "SSLSession 写包头失败，错误代码:" + boost::lexical_cast<std::string>(error.value()) + ", 错误消息:" + error.message());

		close();
		return;
	}


	if (transferredBytes != resp->GetMsgHeaderSize())
	{
		gFileLog::instance().error("network", "SSLSession 写包头失败 需要写:" + boost::lexical_cast<std::string>(resp->GetMsgHeaderSize()) + ", 实际写:" + boost::lexical_cast<std::string>(transferredBytes) );

		close();
		return;
	}

	
		boost::asio::async_write(socket,
			boost::asio::buffer(resp->GetMsgContent(), resp->GetMsgContentSize()),
			boost::asio::transfer_all(),
			strand.wrap(
				boost::bind(&SSLSession::OnWriteMsg, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, resp)
			)
		);
	
	
}

void SSLSession::OnWriteMsg(const boost::system::error_code& error, size_t transferredBytes, IMessage* resp)
{

	if (error)
	{

		gFileLog::instance().error("network", "SSLSession 写包内容失败， 错误代码:" + boost::lexical_cast<std::string>(error.value()) + ", 错误消息:" + error.message());

		close();
		return;
	}

	if (transferredBytes != resp->GetMsgContentSize())
	{
		gFileLog::instance().error("network", "SSLSession 写包内容失败 需要写:" + boost::lexical_cast<std::string>(resp->GetMsgContentSize()) + ", 实际写:" + boost::lexical_cast<std::string>(error) );

		close();
		return;
	}

	// 存入日志队列
	resp->SetSendTime();

	// 写日志
	gFileLogManager::instance().push(resp->log);
	gDistributedLogManager::instance().push(resp->log);

	/*
	if (resp->GetMsgHeader()->FunctionNo == 0)
	{
		// 心跳功能不写日志
	}
	else
	{
		
	}
	*/

	// 删除应答包
	resp->destroy();
}
