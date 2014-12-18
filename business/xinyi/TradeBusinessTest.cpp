#include "stdafx.h"


#include "TradeBusinessTest.h"

#include "FileLog.h"
#include "counter.h"
#include "ConfigManager.h"
#include "ConnectTest.h"
#include "ConnectManager.h"


TradeBusinessTest::TradeBusinessTest(void)
{
	
}

TradeBusinessTest::TradeBusinessTest(int connId, Counter counter)
{
	this->connId = connId;
	this->counter = counter;
}

TradeBusinessTest::~TradeBusinessTest(void)
{
}



bool TradeBusinessTest::Write(CustomMessage * pReq)
{
	if (!WriteMsgHeader(pReq))
		return false;

	if (!WriteMsgContent(pReq))
		return false;

	return true;
}

// 写包头
bool TradeBusinessTest::WriteMsgHeader(CustomMessage * pReq)
{
	if (!Send(pReq->GetMsgHeader().data(), sizeof(MSG_HEADER), 0) )
	{

		//std::string sErrInfo = "新意服务器写包头失败，错误代码：" + "sErrCode" + ", 错误消息：" + "sErrMsg";
		std::string sErrInfo = "新意服务器写包头失败";
		gFileLog::instance().Log(sErrInfo);

		m_bConnected = false;
		return m_bConnected;
	}

	m_bConnected = true;
	return m_bConnected;
}

bool TradeBusinessTest::WriteMsgContent(CustomMessage * pReq)
{
	if (!Send(pReq->GetMsgContent().data(), pReq->GetMsgContentSize(), 0) )
	{
		
		//std::string sErrInfo = "新意服务器写包内容失败，错误代码：" + sErrCode + ", 错误消息：" + sErrMsg;
		std::string sErrInfo = "新意服务器写包内容失败";
		gFileLog::instance().Log(sErrInfo);

		m_bConnected = false;
		return m_bConnected;
	}

	m_bConnected = true;
	return m_bConnected;
}

bool TradeBusinessTest::Read(CustomMessage * pRes)
{
	if (!ReadMsgHeader(pRes))
		return false;

	if (!ReadMsgContent(pRes))
		return false;

	
	return true;
}

// 读包头
bool TradeBusinessTest::ReadMsgHeader(CustomMessage * pRes)
{
	if (!Recv(pRes->GetMsgHeader().data(), sizeof(MSG_HEADER), 0) )
	{
		
		//std::string sErrInfo = "新意服务器读包头失败，错误代码：" + sErrCode + ", 错误消息：" + sErrMsg;
		std::string sErrInfo = "新意服务器读包头失败";
		gFileLog::instance().Log(sErrInfo);

		m_bConnected = false;
		return m_bConnected;
	}

	m_bConnected = true;
	return m_bConnected;
}

// 读包内容
bool TradeBusinessTest::ReadMsgContent(CustomMessage * pRes)
{
	

	if (!pRes->DecoderMsgHeader())
	{
		return false;
	}
		

	if (!Recv(pRes->GetMsgContent().data(), pRes->GetMsgContentSize(), 0) )
	{
		
		//std::string sErrInfo = "新意服务器读包内容失败，错误代码：" + sErrCode + ", 错误消息：" + sErrMsg;
		std::string sErrInfo = "新意服务器读包内容失败";
		gFileLog::instance().Log(sErrInfo);

		m_bConnected = false;
		return m_bConnected;			
	}
		
	m_bConnected = true;
	return m_bConnected;
}



bool TradeBusinessTest::Send(std::string& request, std::string& response, int& status, std::string& errCode, std::string& errMsg)
{
	// 临时修改，无论新意服务器业务处理成功，还是失败，都不让TradeServer处理逻辑

	ParseRequest(request);

	

	bool bRet = true;
	

	// 发送请求
	CustomMessage * pReq = new CustomMessage(MSG_TYPE_TCP_NEW);

	

	pReq->SetMsgContent(request);

	MSG_HEADER binRespMsgHeader;
	binRespMsgHeader.CRC = 0;
	binRespMsgHeader.FunctionNo = 1;
	binRespMsgHeader.MsgContentSize = request.size();
	binRespMsgHeader.MsgType = MSG_TYPE_REQUEST;
	binRespMsgHeader.zip = 0;
	memcpy(&(pReq->msgHeader.front()), &binRespMsgHeader, sizeof(MSG_HEADER));

	//pReq->SetMsgHeader();

	int temp = pReq->GetMsgHeaderSize();

	bRet = Write(pReq);
	delete pReq;
	if (!bRet)
	{
		status = 0;
		errCode = "";
		errMsg = "写包头或包消息失败";

		return false;
	}

	// 接收应答
	CustomMessage* pRes = new CustomMessage(MSG_TYPE_TCP_NEW);
	bRet = Read(pRes);
	if (bRet)
	{
		status = 1;
		response = pRes->GetMsgContentString();
		//std::string response(pRes->GetPkgBody().begin(),pRes->GetPkgBody().end());
		gFileLog::instance().Log("新意服务器应答内容：" + pRes->GetMsgContentString());
	}
	else
	{
		status = 0;
		errCode = "";
		errMsg = "读包头或包消息失败";
	}
	delete pRes;	
	
	return bRet;
}


int TradeBusinessTest::Send(const char * buf, int len, int flags)
{
	int rc = 0;
	
	int totalBytes = 0;

	do
	{
		int bytes = 0;
		bytes = send(sockfd, buf + totalBytes, len - totalBytes, flags);
		if (bytes == 0)
		{
			// connection is closed
			break;
		}

		if (bytes < 0)
		{
			break;
		}

		totalBytes += bytes;


	} while(totalBytes < len);

	if (totalBytes != len)
		rc = FALSE;
	else
		rc = TRUE;

	return rc;
}

int TradeBusinessTest::Recv(char* buf, int len, int flags)
{
	int rc = 0;
	int totalBytes = 0;

	do
	{
		int bytes = 0;
		bytes = recv(sockfd, buf + totalBytes, len - totalBytes, flags);
		if (bytes == 0)
		{
			// connection is closed
			break;
		}

		if (bytes < 0)
		{
			break;
		}

		totalBytes += bytes;


	} while(totalBytes < len);

	if (totalBytes != len)
		rc = FALSE;
	else
		rc = TRUE;

	return rc;
}

bool TradeBusinessTest::CreateConnect()
{
	int rc = 0;

	u_long bio = 1;

	sockfd = INVALID_SOCKET;

	int connectTimeout = counter.m_nConnectTimeout;
	int readTimeout = counter.m_nRecvTimeout * 1000;
	int writeTimeout = counter.m_nRecvTimeout * 1000;
	


	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd == INVALID_SOCKET )
	{
		return FALSE;
	}
	
	//设置为非阻塞模式
	bio = 1;
	rc = ioctlsocket(sockfd, FIONBIO, &bio); 
	if (rc == SOCKET_ERROR)
	{
		closesocket(sockfd);
		return FALSE;
	}
	
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(counter.m_sIP.c_str());
	addr.sin_port = htons(counter.m_nPort);

	rc = connect(sockfd, (const sockaddr *)&addr, sizeof(addr));
	// 异步模式不用判断
	if (rc == SOCKET_ERROR)
	{
		//closesocket(sockfd);
		//return FALSE;
	}

	
	
	
	fd_set writefds;
	FD_ZERO(&writefds);
	FD_SET(sockfd, &writefds);

	struct timeval timeout;
	timeout.tv_sec = connectTimeout;
	timeout.tv_usec = 0;

	rc = select(0, NULL, &writefds, NULL, &timeout);
	if (rc == 0)
	{
		// timeout
		closesocket(sockfd);
		return FALSE;
	}

	if (rc == SOCKET_ERROR)
	{
		closesocket(sockfd);
		return FALSE;
	}

	if(!FD_ISSET(sockfd, &writefds))  
    {  
		closesocket(sockfd);
		return FALSE;
    }  

	 
	// 设置为阻塞模式
	bio = 0;
	rc = ioctlsocket(sockfd, FIONBIO, &bio);
	if (rc == SOCKET_ERROR)
	{
		closesocket(sockfd);
		return FALSE;
	}

	// 设置读写超时
	//rc = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,(char*)&readTimeout, sizeof(readTimeout));
    //rc = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO,(char*)&writeTimeout, sizeof(writeTimeout));

	return TRUE;
}

void TradeBusinessTest::CloseConnect()
{
	if (sockfd != INVALID_SOCKET)
	{
		closesocket(sockfd);
		sockfd = INVALID_SOCKET;
	}
	
	//m_bConnected = false;
}