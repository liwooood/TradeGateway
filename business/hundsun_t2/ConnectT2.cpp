#include "stdafx.h"


#include <string>

#include <boost/format.hpp>

#include "ConnectT2.h"
#include "ConfigManager.h"
#include "common.h"
#include "FileLog.h"
#include "errcode.h"


ConnectT2::ConnectT2(int ConnectNo, Counter counter)
{
	lpConnection = NULL;
	lpConfig = NULL;

	m_nID = ConnectNo;
	m_Counter = counter;

	boost::format fmt("序号%1% 柜台地址%2%:%3%");
	fmt % m_nID % m_Counter.m_sIP % m_Counter.m_nPort;
	m_sServerInfo = fmt.str();



	// 创建连接中断事件, 手工触发，初始状态无信号
	hCloseEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	// 向回调类传递事件
	callback.SetCloseEvent(hCloseEvent);

	// 创建应答事件, 手工触发，初始状态无信号
	hResponseEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	// 向回调类传递事件
	callback.SetResponseEvent(hResponseEvent);

	// 创建判断连接是否断开的线程
	HANDLE hThread = CreateThread(NULL, 0, ConnectT2::AutoConnect, this, 0, NULL);
	CloseHandle(hThread);

}


ConnectT2::~ConnectT2(void)
{
	CloseHandle(hCloseEvent);
	CloseHandle(hResponseEvent);

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
				gFileLog::instance().Log("恒生t2异步，柜台中断重连失败" + sErrMsg);

                // 避免连接太过频繁，Sleep一下
                Sleep(100);
            }
            else
            {
				gFileLog::instance().Log("恒生t2异步，柜台中断重连成功");

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

	

	for (int i=0; i<nRetry; i++)
	{
		nRet = lpConnection->Create2BizMsg(&callback);
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


bool ConnectT2::Send(std::string request)
{
	bool bRet = true;
	int nRet = 0;
	
	
	

	ParseRequest(request);

	// 传递参数
	this->request = request;
	
	funcId = funcid;
	callback.SetRequest(request, funcId, account);


	if (route.empty())
	{
		nRoute = -1;
	}
	else
	{
		try
		{
			//gFileLog::instance().Log("恒生T2 异步模式route=" + route);

			nRoute = boost::lexical_cast<int>(route);
		}
		catch(std::exception& e)
		{
			//gFileLog::instance().Log("恒生T2 异步模式 route异常");

			e.what();

			GenResponse(PARAM_ERROR, gError::instance().GetErrMsg(PARAM_ERROR), response, status, errCode, errMsg);

			bRet = false;	
			goto FINISH;
		}
	}

	
	try
	{
		//gFileLog::instance().Log("恒生T2 异步模式funcid=" + funcid);

		lFuncId = boost::lexical_cast<long>(funcid);
	}
	catch(std::exception& e)
	{
		//gFileLog::instance().Log("恒生T2 异步模式 funcid异常");

		e.what();

		GenResponse(PARAM_ERROR, gError::instance().GetErrMsg(PARAM_ERROR), response, status, errCode, errMsg);
		bRet = false;	
		goto FINISH;
	}



	IBizMessage* lpBizMessage = NewBizMessage();
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction(lFuncId);
	lpBizMessage->SetPacketType(REQUEST_PACKET);

	IF2Packer* lpPacker = NewPacker(2);
	lpPacker->AddRef();
	lpPacker->BeginPack();

	// 设置key
	for (std::vector<std::string>::iterator it = keys.begin(); it != keys.end(); it++)
	{
		std::string key = *it;

		//pack->AddField(key.c_str());
		lpPacker->AddField(key.c_str(), 'S', 8000);
	}
	/*
	for (std::map<std::string, std::string>::iterator it = reqmap.begin(); it != reqmap.end(); it++)
	{
		std::string key = it->first;
		std::string value = it->second;

		

		// 如果是南京证券
		if (sysNo.find("njzq") != std::string::npos)
		{
			if (key == "op_station")
			{
				value = note;
			}
		}

		lpPacker->AddStr(value.c_str());

	} // end for
	*/

	
	// 设置value
	if (num > 1)
	{
		std::string sNum = "num=" + boost::lexical_cast<std::string>(num) + request;
		gFileLog::instance().Log(sNum, 0, "num.log");
	}

	for (int i=0; i<num; i++)
	{

		for (std::vector<std::string>::iterator it = keys.begin(); it != keys.end(); it++)
		{
			std::string key = *it;

			if (i != 0)
			{
				key = key + "_" + boost::lexical_cast<std::string>(i);
			}

			std::string value = reqmap[key];
			TRACE("key=");
			TRACE(key.c_str());
			TRACE(", value=");
			TRACE(value.c_str());
			TRACE("\n");

			
			// 如果是南京证券
			if (sysNo.find("njzq") != std::string::npos)
			{
				if (key == "op_station")
				{
					value = note;
				}
			}

			lpPacker->AddStr(value.c_str());
			
		}
	}


	lpPacker->EndPack();


	lpBizMessage->SetContent(lpPacker->GetPackBuf(),lpPacker->GetPackLen());
	nRet = lpConnection->SendBizMsg(lpBizMessage, 1); //  1代表异步
	

	if (nRet < 0)
	{
		//gFileLog::instance().Log("恒生T2 异步模式 SendBiz");

		lpPacker->FreeMem(lpPacker->GetPackBuf());
		lpPacker->Release();

		lpBizMessage->Release();

		
		GenResponse(nRet, lpConnection->GetErrorMsg(nRet), response, status, errCode, errMsg);
		bRet = false;
		goto FINISH;
	}


	lpPacker->FreeMem(lpPacker->GetPackBuf());
	lpPacker->Release();

	lpBizMessage->Release();

	

	
FINISH:
	

	return bRet;
}


T2_ASYNC_RET ConnectT2::GetSendResponse()
{
	T2_ASYNC_RET ret;

	ret.request = request;
	ret.funcId = funcid;
	ret.account = account;

	ret.status = status;
	ret.response = response;
	ret.errCode = errCode;
	ret.errMsg = errMsg;
	

	return ret;
}

DWORD ConnectT2::WaitResponseEvent()
{
	DWORD dwResult = WaitForSingleObject(hResponseEvent, readWriteTimeout);

	return dwResult;
}


T2_ASYNC_RET ConnectT2::GetAsyncResponse()
{
	return callback.getResponse();
	
}

bool ConnectT2::Send(std::string& request, std::string& response, int& status, std::string& errCode, std::string& errMsg)
{
	return false;
}
