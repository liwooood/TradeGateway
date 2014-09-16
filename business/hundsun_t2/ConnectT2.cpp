#include "stdafx.h"


#include <string>

#include <boost/format.hpp>

#include "ConnectT2.h"
#include "./config/ConfigManager.h"
#include "common.h"
#include "./output/FileLog.h"



ConnectT2::ConnectT2(int ConnectNo, Counter counter)
{
	init();

	m_nID = ConnectNo;
	m_Counter = counter;

	boost::format fmt("序号%1% 柜台地址%2%:%3%");
	fmt % m_nID % m_Counter.m_sIP % m_Counter.m_nPort;
	m_sServerInfo = fmt.str();

}


ConnectT2::~ConnectT2(void)
{
	CloseConnect();
}


void ConnectT2::init()
{
	lpConnection = NULL;
	lpConfig = NULL;
}

std::string ConnectT2::GetConnectInfo()
{
	return m_sServerInfo;
}


bool ConnectT2::CreateConnect()
{
	int nRetry = gConfigManager::instance().m_nConnectRetry;


	int nRet = 0;

	lpConfig = NewConfig();
	lpConfig->AddRef();

	std::string s = m_Counter.m_sIP;
	s += ":";
	s += boost::lexical_cast<std::string>(m_Counter.m_nPort);
	lpConfig->SetString("t2sdk", "servers", s.c_str());

	std::string license_file;
	license_file = gConfigManager::instance().m_sPath + "\\license.dat";
	lpConfig->SetString("t2sdk", "license_file", license_file.c_str());
	lpConfig->SetString("t2sdk", "lang", "1033");
	lpConfig->SetString("t2sdk", "send_queue_size", "100");
	lpConfig->SetString("safe", "safe_level", "none");

	for (int i=0; i<nRetry; i++)
	{
		lpConnection = NewConnection(lpConfig);
		lpConnection->AddRef();

		nRet = lpConnection->Create(&callback);

		
		nRet = lpConnection->Connect(m_Counter.m_nConnectTimeout*1000);

		if (nRet != 0)
		{
			// 连接连接失败
			std::string sErrMsg = lpConnection->GetErrorMsg(nRet);
			std::string msg = "建立连接失败, " + m_sServerInfo + sErrMsg;
			gFileLog::instance().Log(msg);

			Sleep(500);

			continue;
		}
		else
		{
			

			std::string msg = "建立连接成功, " + m_sServerInfo;
			
			gFileLog::instance().Log(msg);

			return true;
		}
	} // end for

	return false;
}



bool ConnectT2::ReConnect()
{
	CloseConnect();

	bool bRet = CreateConnect();

	if (bRet)
	{
		std::string msg = "重连成功, " + m_sServerInfo;
		gFileLog::instance().Log(msg);
	}
	else
	{
		std::string msg = "重连失败, " + m_sServerInfo;
		gFileLog::instance().Log(msg);
	}

	return bRet;
}


void ConnectT2::CloseConnect()
{
	if (lpConnection != NULL)
	{
		std::string msg = "关闭恒生t2连接，" + m_sServerInfo;
		gFileLog::instance().Log(msg);

		lpConnection->Release();
		//delete lpConnection;
		lpConnection = NULL;
	}

	if (lpConfig != NULL)
	{
		lpConfig->Release();
		//delete lpConfig;
		lpConfig = NULL;
	}
}
