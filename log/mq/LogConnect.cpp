#include "stdafx.h"

#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
//#include <boost/format.hpp>

#include "LogConnect.h"
#include "FileLog.h"



LogConnect::LogConnect(void)
	:socket(ios)
	,deadline(ios)
{
	logFileName = "�ֲ�ʽ��־";

	m_bConnected = false;

	// Ĭ�����ӳ�ʱʱ��
	connectTimeout = 1;

	// Ĭ�϶�д��ʱʱ��
	readWriteTimeout = 5;



	deadline.expires_at(boost::posix_time::pos_infin);
	
	check_deadline();
}

LogConnect::~LogConnect(void)
{
}

// ��ʱ�ص�����
void LogConnect::check_deadline()
{
	
	if (deadline.expires_at() <= boost::asio::deadline_timer::traits_type::now())
	{
		gFileLog::instance().Log("���ӳ�ʱ���д��ʱ", 0, logFileName);

		Close();

		deadline.expires_at(boost::posix_time::pos_infin);
	}
	
	deadline.async_wait( boost::bind(&LogConnect::check_deadline, this) );
}

bool LogConnect::Connect(std::string server)
{
	std::vector<std::string> kv;
	boost::split(kv, server, boost::is_any_of(":")); // ע����Ҫͨ�������ļ�����

	m_sIP = kv[0];
	m_nPort = boost::lexical_cast<int>(kv[1]);

	return Connect(m_sIP, m_nPort);
}

// ��������
bool LogConnect::Connect(std::string ip, int port)
{
	

	try
	{
		m_bConnected = false;
	
		m_sIP = ip;
		m_nPort = port;

		boost::system::error_code ec;

		boost::asio::ip::tcp::resolver resolver(ios);

		boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), m_sIP, boost::lexical_cast<std::string>(m_nPort));

		boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query, ec);

		// �������ӳ�ʱ
		
		deadline.expires_from_now( boost::posix_time::seconds(connectTimeout*1000) );

		ec = boost::asio::error::would_block;

		boost::asio::async_connect(socket, iterator, boost::lambda::var(ec) = boost::lambda::_1);
	
		do 
			ios.run_one(); 
		while (ec == boost::asio::error::would_block);

		if (ec || !socket.is_open())
		{
			std::string sErrCode = boost::lexical_cast<std::string>(ec.value());
			std::string sErrMsg = ec.message();
			std::string sErrInfo = "������־������ʧ�ܣ�������룺" + sErrCode + ", ������Ϣ��" + sErrMsg;
			gFileLog::instance().Log(sErrInfo, 0, logFileName);
			
			
			m_bConnected = false;
			return m_bConnected;
		}

		gFileLog::instance().Log("������־�������ɹ�!", 4, logFileName);
		m_bConnected = true;
		return m_bConnected;
	}
	catch(std::exception& e)
	{
		gFileLog::instance().Log("������־�������쳣��" + std::string(e.what()), 0, logFileName);
		m_bConnected = false;
		return m_bConnected;
	}
}

bool LogConnect::IsConnected()
{

	
	return m_bConnected && socket.is_open();
}



bool LogConnect::Write(CustomMessage * pReq)
{
	if (!WriteMsgHeader(pReq))
		return false;

	if (!WriteMsgContent(pReq))
		return false;

	return true;
}

// д��ͷ
bool LogConnect::WriteMsgHeader(CustomMessage * pReq)
{
	boost::system::error_code ec = boost::asio::error::would_block;

	
	
	boost::asio::async_write(socket, 
		boost::asio::buffer(pReq->GetMsgHeader(), sizeof(MSG_HEADER)), 
		boost::asio::transfer_all(), 
		boost::lambda::var(ec) = boost::lambda::_1);
	
	do 
		ios.run_one(); 
	while (ec == boost::asio::error::would_block);

	if (ec)
	{
		std::string sErrCode = boost::lexical_cast<std::string>(ec.value());
		std::string sErrMsg = ec.message();
		std::string sErrInfo = "д��ͷʧ�ܣ�������룺" + sErrCode + ", ������Ϣ��" + sErrMsg;
		gFileLog::instance().Log(sErrInfo, 0,logFileName);

		m_bConnected = false;
		return m_bConnected;
	}

	m_bConnected = true;
	return m_bConnected;
}

bool LogConnect::WriteMsgContent(CustomMessage * pReq)
{
	boost::system::error_code ec = boost::asio::error::would_block;

	boost::asio::async_write(socket, 
		boost::asio::buffer(pReq->GetMsgContent(), pReq->GetMsgContentSize()), 
		boost::asio::transfer_all(), 
		boost::lambda::var(ec) = boost::lambda::_1);
	
	do 
		ios.run_one(); 
	while (ec == boost::asio::error::would_block);

	if (ec)
	{
		std::string sErrCode = boost::lexical_cast<std::string>(ec.value());
		std::string sErrMsg = ec.message();
		std::string sErrInfo = "д������ʧ�ܣ�������룺" + sErrCode + ", ������Ϣ��" + sErrMsg;
		gFileLog::instance().Log(sErrInfo, 0,logFileName);

		m_bConnected = false;
		return m_bConnected;
	}

	m_bConnected = true;
	return m_bConnected;
}

bool LogConnect::Read(CustomMessage * pRes)
{
	if (!ReadMsgHeader(pRes))
		return false;

	if (!ReadMsgContent(pRes))
		return false;

	
	return true;
}

// ����ͷ
bool LogConnect::ReadMsgHeader(CustomMessage * pRes)
{
	boost::system::error_code ec = boost::asio::error::would_block;

	boost::asio::async_read(socket, 
		boost::asio::buffer(pRes->GetMsgHeader(), sizeof(MSG_HEADER)), 
		boost::asio::transfer_all(), 
		boost::lambda::var(ec) = boost::lambda::_1);
	do 
		ios.run_one(); 
	while (ec == boost::asio::error::would_block);

	if (ec)
	{
		std::string sErrCode = boost::lexical_cast<std::string>(ec.value());
		std::string sErrMsg = ec.message();
		std::string sErrInfo = "����ͷʧ�ܣ�������룺" + sErrCode + ", ������Ϣ��" + sErrMsg;
		gFileLog::instance().Log(sErrInfo, 0, logFileName);

		m_bConnected = false;
		return m_bConnected;
	}

	m_bConnected = true;
	return m_bConnected;
}

// ��������
bool LogConnect::ReadMsgContent(CustomMessage * pRes)
{
	boost::system::error_code ec = boost::asio::error::would_block;

	if (!pRes->DecoderMsgHeader())
	{
		return false;
	}
		
	boost::asio::async_read(socket, 
		boost::asio::buffer(pRes->GetMsgContent(), pRes->GetMsgContentSize()),
		boost::asio::transfer_all(), 
		boost::lambda::var(ec) = boost::lambda::_1);
	do 
		ios.run_one(); 
	while (ec == boost::asio::error::would_block);

		
	if (ec)
	{
		std::string sErrCode = boost::lexical_cast<std::string>(ec.value());
		std::string sErrMsg = ec.message();
		std::string sErrInfo = "��������ʧ�ܣ�������룺" + sErrCode + ", ������Ϣ��" + sErrMsg;
		gFileLog::instance().Log(sErrInfo, 0, logFileName);

		m_bConnected = false;
		return m_bConnected;			
	}
		
	m_bConnected = true;
	return m_bConnected;
}

// �ر�����
void LogConnect::Close()
{
	m_bConnected = false;

	boost::system::error_code ec;

	

	socket.close(ec);
	
	if (ec)
	{
		gFileLog::instance().Log("�Ͽ���־�������쳣��" + ec.message(), 0,logFileName);
	}

	
	gFileLog::instance().Log("�Ͽ���־������!", 4,logFileName);
}

bool LogConnect::ReConnect()
{
	Close();

	return Connect(this->m_sIP, this->m_nPort);
}

// ����������
bool LogConnect::HeartBeat()
{
	bool bRet = false;

	/*
	if (!m_bConnected)
		return false;

	std::string SOH = "\x01";

	std::string request = "cssweb_funcid=999999" + SOH;

	

	
	
	// ���ö�д��ʱ
	//int nReadWriteTimeout = sConfigManager::instance().m_nReadWriteTimeout;
	//deadline.expires_from_now( boost::posix_time::seconds(nReadWriteTimeout) );

	// ��������
	CustomMessage * pReq = new CustomMessage();

	

	pReq->SetMsgContent(request);
	pReq->SetMsgHeader(MSG_TYPE_REQUEST, FUNCTION_HEARTBEAT);

	int temp = pReq->GetMsgHeaderSize();

	bRet = Write(pReq);
	delete pReq;
	if (!bRet)
		return false;

	// ����Ӧ��
	CustomMessage * pRes = new CustomMessage();
	bRet = Read(pRes);
	if (bRet)
	{
		//std::string response(pRes->GetPkgBody().begin(),pRes->GetPkgBody().end());
		gFileLog::instance().Log("Ӧ�����ݣ�" + pRes->GetMsgContentString());
	}
	else
	{
	}
	delete pRes;	

	*/

	return bRet;
}

void LogConnect::SetConnectTimeout(int connecTimeout)
{
	this->connectTimeout = connectTimeout;
}

void LogConnect::SetReadWriteTimeout(int readWriteTimeout)
{
	this->readWriteTimeout = readWriteTimeout;
}

bool LogConnect::Send(std::string& request, std::string& response)
{
	bool bRet = false;

	
	deadline.expires_from_now( boost::posix_time::seconds(readWriteTimeout*1000) );

	// ��������
	CustomMessage * pReq = new CustomMessage(MSG_TYPE_TCP_NEW);

	

	pReq->SetMsgContent(request);

	MSG_HEADER binRespMsgHeader;
	binRespMsgHeader.CRC = 0;
	binRespMsgHeader.FunctionNo = 1;
	binRespMsgHeader.MsgType = MSG_TYPE_REQUEST;
	binRespMsgHeader.zip = 0;
	binRespMsgHeader.MsgContentSize = request.size();
	memcpy(&(pReq->msgHeader.front()), &binRespMsgHeader, sizeof(MSG_HEADER));


	int temp = pReq->GetMsgHeaderSize();

	bRet = Write(pReq);
	delete pReq;
	if (!bRet)
		return false;

	// ����Ӧ��
	CustomMessage * pRes = new CustomMessage(MSG_TYPE_TCP_NEW);
	bRet = Read(pRes);
	if (bRet)
	{
		response = pRes->GetMsgContentString();
		
	}
	else
	{
		// ʧ��
	}
	delete pRes;	
	

	return bRet;
}
