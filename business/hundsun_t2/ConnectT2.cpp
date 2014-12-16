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

	boost::format fmt("���%1% ��̨��ַ%2%:%3%");
	fmt % m_nID % m_Counter.m_sIP % m_Counter.m_nPort;
	m_sServerInfo = fmt.str();



	// ���������ж��¼�, �ֹ���������ʼ״̬���ź�
	hCloseEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	// ��ص��ഫ���¼�
	callback.SetCloseEvent(hCloseEvent);

	// ����Ӧ���¼�, �ֹ���������ʼ״̬���ź�
	hResponseEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	// ��ص��ഫ���¼�
	callback.SetResponseEvent(hResponseEvent);

	// �����ж������Ƿ�Ͽ����߳�
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

// �ж������Ƿ��жϵ��̺߳���
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
				gFileLog::instance().Log("����t2�첽����̨�ж�����ʧ��" + sErrMsg);

                // ��������̫��Ƶ����Sleepһ��
                Sleep(100);
            }
            else
            {
				gFileLog::instance().Log("����t2�첽����̨�ж������ɹ�");

                // �����ɹ��������ڲ�ѭ�����ȴ��Ͽ��¼��ٴα�����
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
			// �ɹ�
			

			nRet = lpConnection->Connect(connectTimeout);

			if (nRet != 0)
			{
				// ��������ʧ��
				sErrMsg = lpConnection->GetErrorMsg(nRet);

				std::string msg = "��������ʧ��, " + m_sServerInfo + sErrMsg;
				gFileLog::instance().Log(msg);

				Sleep(100);

				continue;
			}
			else
			{
			

				std::string msg = "�������ӳɹ�, " + m_sServerInfo;
			
				gFileLog::instance().Log(msg);

				return true;
			}
		}
		else
		{
			// ʧ��
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
		std::string msg = "�����ɹ�, " + m_sServerInfo;
		gFileLog::instance().Log(msg);
	}
	else
	{
		std::string msg = "����ʧ��, " + m_sServerInfo;
		gFileLog::instance().Log(msg);
	}

	return bRet;
}
*/

void ConnectT2::CloseConnect()
{
	if (lpConnection != NULL)
	{
		std::string msg = "�رպ���t2���ӣ�" + m_sServerInfo;
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

	// ���ݲ���
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
			//gFileLog::instance().Log("����T2 �첽ģʽroute=" + route);

			nRoute = boost::lexical_cast<int>(route);
		}
		catch(std::exception& e)
		{
			//gFileLog::instance().Log("����T2 �첽ģʽ route�쳣");

			e.what();

			GenResponse(PARAM_ERROR, gError::instance().GetErrMsg(PARAM_ERROR), response, status, errCode, errMsg);

			bRet = false;	
			goto FINISH;
		}
	}

	
	try
	{
		//gFileLog::instance().Log("����T2 �첽ģʽfuncid=" + funcid);

		lFuncId = boost::lexical_cast<long>(funcid);
	}
	catch(std::exception& e)
	{
		//gFileLog::instance().Log("����T2 �첽ģʽ funcid�쳣");

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

	// ����key
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

		

		// ������Ͼ�֤ȯ
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

	
	// ����value
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

			
			// ������Ͼ�֤ȯ
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
	nRet = lpConnection->SendBizMsg(lpBizMessage, 1); //  1�����첽
	

	if (nRet < 0)
	{
		//gFileLog::instance().Log("����T2 �첽ģʽ SendBiz");

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
