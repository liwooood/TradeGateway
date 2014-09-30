#ifndef _IMESSAGE_H_
#define _IMESSAGE_H_

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "sslsession.h"
#include "tcpsession.h"

#include "tradelog.pb.h"

#define MSG_TYPE_HTTP 0
#define MSG_TYPE_TCP_OLD 1
#define MSG_TYPE_SSL_PB 2
#define MSG_TYPE_TCP_NEW 3
#define MSG_TYPE_SSL_NEW 4


//class TcpSession;
//class SSLSession;

class IMessage
{
public:
	IMessage();
	~IMessage(void);

	// ҵ�������ɵ���־
	// ��־�Ĵ���
	void SetRecvTime();
	void SetSendTime();

	Trade::TradeLog log;
	void Log(Trade::TradeLog::LogLevel logLevel, std::string sysNo, std::string sysVer, std::string busiType, std::string funcId, std::string account, std::string clientIp, std::string request, std::string response, int status, std::string errCode, std::string errMsg, std::string beginTime, int runtime, std::string gatewayIp, std::string gatewayPort, std::string counterIp, std::string counterPort, std::string counterType);

	


	
	size_t m_MsgHeaderSize;
	std::vector<char> m_MsgHeader;
	size_t GetMsgHeaderSize();
	std::vector<char>& GetMsgHeader();
	void SetMsgHeader(std::vector<char> header);
	virtual bool DecoderMsgHeader()=0;


	std::vector<char> m_MsgContent;
	size_t GetMsgContentSize();
	std::vector<char>& GetMsgContent();
	std::string GetMsgContentString();
	void SetMsgContent(std::string content);
	void SetMsgContent(std::vector<char> content);
	


	int m_msgType;
	
	

	// ɾ����Ϣָ��
	void destroy();	


	// ������Ϣ��Ҫ�ع�
	TcpSessionPtr tcpSession;
	void SetTcpSession(TcpSessionPtr session);
	TcpSessionPtr& GetTcpSession();

	SSLSessionPtr sslSession;
	void SetSslSession(SSLSessionPtr session);
	SSLSessionPtr& GetSslSession();
};

#endif
