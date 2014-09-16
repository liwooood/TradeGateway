#include "stdafx.h"


#include <string>

#include <boost/format.hpp>

#include "ConnectT2.h"
#include "./config/ConfigManager.h"
#include "common.h"
#include "./output/FileLog.h"



ConnectT2::ConnectT2(int ConnectNo, Counter counter)
{
	lpConnection = NULL;
	lpConfig = NULL;

	m_nID = ConnectNo;
	m_Counter = counter;

	boost::format fmt("序号%1% 柜台地址%2%:%3%");
	fmt % m_nID % m_Counter.m_sIP % m_Counter.m_nPort;
	m_sServerInfo = fmt.str();

	connectTimeout = m_Counter.m_nConnectTimeout * 1000;
}


ConnectT2::~ConnectT2(void)
{
	CloseConnect();
}



std::string ConnectT2::GetConnectInfo()
{
	return m_sServerInfo;
}

// 判断连接是否中断的线程函数
DWORD WINAPI ConnectT2::AutoConnect(LPVOID lpParam)
{
	ConnectT2* pThis = (ConnectT2*)lpParam;

	for (;;)
    {
		WaitForSingleObject(pThis->hCloseEvent, INFINITE);

        for (;;)
        {
			int ret = 0;

			if (ret = pThis->lpConnection->Connect(pThis->connectTimeout))
            {
               
                std::string sErrMsg =  pThis->lpConnection->GetErrorMsg(ret);

                // 避免连接太过频繁，Sleep一下
                Sleep(100);
            }
            else
            {
                // 重连成功，跳出内层循环，等待断开事件再次被激活
                break;
            }
        }
    }

    return 0;
}

bool ConnectT2::CreateConnect()
{
	std::string sErrMsg = "";
	int nRet = 0;

	int nRetry = gConfigManager::instance().m_nConnectRetry;
	
	// 创建事件
	hCloseEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	// 向回调类传递事件
	callback.SetCloseEvent(hCloseEvent);

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

	lpConnection = NewConnection(lpConfig);
	lpConnection->AddRef();

	
	HANDLE hThread = CreateThread(NULL, 0, ConnectT2::AutoConnect, this, 0, NULL);
	CloseHandle(hThread);


	for (int i=0; i<nRetry; i++)
	{
		nRet = lpConnection->Create(&callback);
		if (nRet == 0)
		{
			// 成功
			

			nRet = lpConnection->Connect(connectTimeout);

			if (nRet != 0)
			{
				// 连接连接失败
				sErrMsg = lpConnection->GetErrorMsg(nRet);

				std::string msg = "建立连接失败, " + m_sServerInfo + sErrMsg;
				gFileLog::instance().Log(msg);

				Sleep(100);

				continue;
			}
			else
			{
			

				std::string msg = "建立连接成功, " + m_sServerInfo;
			
				gFileLog::instance().Log(msg);

				return true;
			}
		}
		else
		{
			// 失败
			sErrMsg = lpConnection->GetErrorMsg(nRet);
		}
	} // end for

	return false;
}


/*
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
*/

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
