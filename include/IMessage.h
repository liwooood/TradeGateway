#ifndef IMESSAGE_H
#define IMESSAGE_H

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>



//#include "sslsession.h"
//#include "tcpsession.h"
#include "tradelog.pb.h"


#define MSG_TYPE_HTTP 0
#define MSG_TYPE_TCP_OLD 1
#define MSG_TYPE_SSL_PB 2
#define MSG_TYPE_TCP_NEW 3
#define MSG_TYPE_SSL_NEW 4

class TcpSession;
typedef boost::shared_ptr<TcpSession> TcpSessionPtr;

class SSLSession;
typedef boost::shared_ptr<SSLSession> SSLSessionPtr;


class IMessage
{
public:
	IMessage();
	~IMessage(void);

	void SetRecvTime();
	void SetSendTime();
	void Log(Trade::TradeLog::LogLevel logLevel, std::string sysNo, std::string sysVer, std::string busiType, std::string funcId, std::string account, std::string clientIp, std::string request, std::string response, int status, std::string errCode, std::string errMsg, std::string beginTime, int runtime, std::string gatewayIp, std::string gatewayPort, std::string counterIp, std::string counterPort, std::string counterType);

	std::vector<char>& GetMsgHeader();

	void SetMsgHeader(std::vector<char> header);

	size_t GetMsgHeaderSize();
	virtual bool DecoderMsgHeader()=0;


	

	std::vector<char>& GetMsgContent();
	void SetMsgContent(std::vector<char> content);
	std::string GetMsgContentString();
	void SetMsgContent(std::string content);
	
	
	size_t GetMsgContentSize();


	
	
	

	// 删除消息指针
	void destroy();	


	// 以下信息需要重构
	//ISession session;
	//SessionPtr GetSession();
	//void SetSesstion(SessionPtr);

	
	void SetTcpSession(TcpSessionPtr& session);
	TcpSessionPtr& GetTcpSession();

	
	void SetSslSession(SSLSessionPtr& session);
	SSLSessionPtr& GetSslSession();

protected:
	
	

	
	SSLSessionPtr sslSession;
	TcpSessionPtr tcpSession;

	

public:
	std::vector<char> msgHeader;
	std::vector<char> msgContent;


	int msgType;
	Trade::TradeLog log;

	int GetMsgType();
};

#endif
