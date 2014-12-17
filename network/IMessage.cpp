#include "stdafx.h"

#include <boost/checked_delete.hpp>

#include "IMessage.h"



IMessage::IMessage()
{

}


IMessage::~IMessage(void)
{
}

TcpSessionPtr& IMessage::GetTcpSession()
{
	return tcpSession;
}
void IMessage::SetTcpSession(TcpSessionPtr& sess)
{
	tcpSession = sess;
}

SSLSessionPtr& IMessage::GetSslSession()
{
	return sslSession;
}
void IMessage::SetSslSession(SSLSessionPtr& sess)
{
	sslSession = sess;
}


std::vector<char>& IMessage::GetMsgHeader()
{
	return msgHeader;
}

size_t IMessage::GetMsgHeaderSize()
{
	return msgHeader.size();
}

void IMessage::SetMsgHeader(std::vector<char> header)
{
	msgHeader = header;
}




std::vector<char>& IMessage::GetMsgContent()
{
	return msgContent;
}


std::string IMessage::GetMsgContentString()
{
	std::string str(msgContent.begin(), msgContent.end());

	return str;
}

size_t IMessage::GetMsgContentSize()
{
	return msgContent.size();
}

void IMessage::SetMsgContent(std::string content)
{
	size_t msgContentSize = content.size();

	if (msgContentSize == 0)
		return;

	msgContent.resize(msgContentSize);

	//std::copy(content.begin(), content.end(), back_inserter(m_MsgContent));

	memcpy(msgContent.data(), content.c_str(), msgContentSize);
}

void IMessage::SetMsgContent(std::vector<char> content)
{
	size_t msgContentSize = content.size();

	if (msgContentSize == 0)
		return;

	msgContent.resize(msgContentSize);

	
	msgContent = content;
}


void IMessage::Log(Trade::TradeLog::LogLevel logLevel, std::string sysNo, std::string sysVer, std::string busiType, std::string funcId, std::string account, std::string clientIp, std::string request, std::string response, int status, std::string errCode, std::string errMsg, std::string beginTime, int runtime, std::string gatewayIp, std::string gatewayPort, std::string counterIp, std::string counterPort, std::string counterType)
{
	log.set_level(logLevel);

	log.set_sysno(sysNo);
	log.set_sysver(sysVer);	
	log.set_busitype(busiType);
	log.set_account(account);
	log.set_funcid(funcId);
	log.set_ip(clientIp);

	log.set_begintime(beginTime); 
	log.set_runtime(runtime);


	
	log.set_enc_request(""); // flash
	log.set_request(request);

	log.set_status(status);
	log.set_retcode(errCode);
	log.set_retmsg(errMsg);
	
	
	log.set_response(response);
	log.set_enc_response(""); // flash
	
	log.set_gatewayip(gatewayIp);
	log.set_gatewayport(gatewayPort);

	log.set_gtip(counterIp);
	log.set_gtport(counterPort);
	log.set_countertype(counterType);
}

void IMessage::SetRecvTime()
{
	boost::posix_time::ptime recvTime =  boost::posix_time::microsec_clock::local_time();

	std::string s = boost::gregorian::to_iso_extended_string(recvTime.date()) + " " + boost::posix_time::to_simple_string(recvTime.time_of_day());
	
	log.set_recvtime(s);
}

void IMessage::SetSendTime()
{
	boost::posix_time::ptime sendTime = boost::posix_time::microsec_clock::local_time();

	std::string s = boost::gregorian::to_iso_extended_string(sendTime.date()) + " " + boost::posix_time::to_simple_string(sendTime.time_of_day());
	
	log.set_sendtime(s);
}

void IMessage::destroy()
{
	boost::checked_delete(this);
}

int IMessage::GetMsgType()
{
	return msgType;
}
