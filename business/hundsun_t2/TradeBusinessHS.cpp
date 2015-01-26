#include "StdAfx.h"




#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>


#include "TradeBusinessHS.h"

#include "ConnectManager.h"

#include "ConfigManager.h"


// ICU
//#include <unicode/putil.h>
//#include <unicode/ucnv.h>


#include "FileLog.h"
#include "errcode.h"
#include "FileLogManager.h"



TradeBusinessHS::TradeBusinessHS()
{
	lpConfig = NULL;
	lpConnection = NULL;
}

TradeBusinessHS::TradeBusinessHS(int ConnectNo, Counter counter)
{
	lpConnection = NULL;
	lpConfig = NULL;

	this->connId = ConnectNo;
	this->counter = counter;
	logFile = "��̨\\����";
	connectInfo = "�������" + boost::lexical_cast<std::string>(connId) + ", ��̨��ַ" + counter.serverAddr;
}



bool TradeBusinessHS::CreateConnect()
{
	int nRet = 0;

	lpConfig = NewConfig();
	lpConfig->AddRef();

	std::string s = counter.m_sIP;
	s += ":";
	s += boost::lexical_cast<std::string>(counter.m_nPort);
	lpConfig->SetString("t2sdk", "servers", s.c_str());

	std::string license_file;
	license_file = gConfigManager::instance().m_sPath + "\\license.dat";
	lpConfig->SetString("t2sdk", "license_file", license_file.c_str());
	lpConfig->SetString("t2sdk", "lang", "1033");
	lpConfig->SetString("t2sdk", "send_queue_size", "100");
	lpConfig->SetString("safe", "safe_level", "none");


	lpConnection = NewConnection(lpConfig);
	lpConnection->AddRef();

	nRet = lpConnection->Create(NULL);

		
	nRet = lpConnection->Connect(counter.m_nConnectTimeout);

	if (nRet != 0)
	{
		std::string sErrMsg = lpConnection->GetErrorMsg(nRet);
			std::string msg = "��������ʧ��" + s + ",������Ϣ" + sErrMsg;
			gFileLog::instance().error(logFile, msg);

		m_bConnected = false;
		return false;
	}
	else
	{
		gFileLog::instance().debug(logFile, "�������ӳɹ�" + connectInfo);
		m_bConnected = true;
		return true;
	}
}

void TradeBusinessHS::CloseConnect()
{
	m_bConnected = false;

	if (lpConnection != NULL)
	{
		lpConnection->Release();
		lpConnection = NULL;
	}

	if (lpConfig != NULL)
	{
		lpConfig->Release();
		lpConfig = NULL;
	}

	gFileLog::instance().debug(logFile, "�ر�����" + connectInfo);
}

/*
ֻ������ͨ�Ŵ���ŷ���false
ҵ�����Ҳ����true
*/
bool TradeBusinessHS::Send(std::string& request, std::string& response, int& status, std::string& errCode, std::string& errMsg)
{
	bool bRet = true;
	int nRet = 0;
	

	ParseRequest(request);

	if (route.empty())
	{
		nRoute = -1;
	}
	else
	{
		try
		{
			nRoute = boost::lexical_cast<int>(route);
		}
		catch(std::exception& e)
		{
			e.what();

			GenResponse(PARAM_FORMAT_ERROR, gError::instance().GetErrMsg(PARAM_FORMAT_ERROR) + "cssweb_route", response, status, errCode, errMsg);

			bRet = true;	
			goto FINISH;
		}
	}

	
	try
	{
		lFuncId = boost::lexical_cast<long>(funcid);
	}
	catch(std::exception& e)
	{
		e.what();

		GenResponse(PARAM_FORMAT_ERROR, gError::instance().GetErrMsg(PARAM_FORMAT_ERROR) + "cssweb_funcid", response, status, errCode, errMsg);
		bRet = true;	
		goto FINISH;
	}



	IF2Packer* pack = NewPacker(2);
	pack->AddRef();

	pack->BeginPack();

	// ����key
	for (std::vector<std::string>::iterator it = keys.begin(); it != keys.end(); it++)
	{
		std::string key = *it;

		//pack->AddField(key.c_str());
		pack->AddField(key.c_str(), 'S', 8000);

		//gFileLog::instance().error(logFile, "key1=" + key);
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
			

			
			// ������Ͼ�֤ȯ
			if (sysNo.find("njzq") != std::string::npos)
			{
				if (key == "op_station")
				{
					value = note;
				}
			}

			pack->AddStr(value.c_str());

			//gFileLog::instance().error(logFile, "key2=" + key);
			//gFileLog::instance().error(logFile, "value=" + value);
			
		}
	}

	pack->EndPack();

	

	
	
	//nRet = m_pConn->lpConnection->SendBiz(boost::lexical_cast<int>(funcid), pack, 0, boost::lexical_cast<int>(system)); //  ͬ��
	/*
	GenResponse(0, "��ʱ�˳�", response, status, errCode, errMsg);
		bRet = true;
		goto FINISH;
		*/

	if (nRoute == -1)
		nRet = lpConnection->SendBiz(lFuncId, pack); //  ͬ��
	else
		nRet = lpConnection->SendBiz(lFuncId, pack, 0, nRoute); //  ͬ��

	if (nRet < 0)
	{
		pack->Release();

		
		GenResponse(nRet, lpConnection->GetErrorMsg(nRet), response, status, errCode, errMsg);
		bRet = true;
		goto FINISH;
	}


	pack->FreeMem(pack->GetPackBuf());
	pack->Release();





	// ��������
	void *Pointer = NULL;

	//nRet = m_pConn->lpConnection->RecvBiz(nRet, &Pointer, gConfigManager::instance().m_nConnectTimeout, 0);
	
	
	nRet = lpConnection->RecvBiz(nRet, &Pointer, counter.m_nRecvTimeout);


	// ���سɹ�
	if (nRet == 0)
	{
		IF2UnPacker *lpUnPacker = (IF2UnPacker *)Pointer;

		

		int nRows = lpUnPacker->GetRowCount();
		int nCols = lpUnPacker->GetColCount();

		if (nRows == 0)
		{
			RetNoRecordRes(response, status);

			bRet = true;
			status = 1;
			
			goto FINISH;
		}
		
		
		std::map<std::string, FUNCTION_DESC>::iterator it = gFileLogManager::instance().m_mT2_FilterFunc.find(funcid);
		if (it != gFileLogManager::instance().m_mT2_FilterFunc.end())
		{
			// ��Ӧ���н��������
			if (!it->second.hasResultRet)
			{
				gFileLog::instance().Log(funcid + "��Ӧ�÷��ؽ����");

				RetNoRecordRes(response, status);
				bRet = true;
				status = 1;
				
				goto FINISH;
			}
		}
		

		response = boost::lexical_cast<std::string>(nRows);
		response += SOH;
		response += boost::lexical_cast<std::string>(nCols);
		response += SOH;
	
		for (int i = 0; i < lpUnPacker->GetDatasetCount(); ++i)
		{
			// ���õ�ǰ�����
			lpUnPacker->SetCurrentDatasetByIndex(i);

			for (int c = 0; c < nCols; c++)
			{
				std::string sColName = lpUnPacker->GetColName(c);
					
				response += sColName;
					
				response += SOH;
			} // end all column

			// ��ӡ���м�¼
			for (int r = 0; r < nRows; r++)
			{
				// ��ӡÿ����¼
				for (int c = 0; c < nCols; c++)
				{
					std::string temp = lpUnPacker->GetStrByIndex(c);

					// ��̩����һ��ͨ���⴦��
					if (funcid == "337014")
						boost::algorithm::replace_all(temp, SOH, "");
					
					response += temp;
					
					response += SOH;
				} // end all column

				lpUnPacker->Next();
			} // end for all row

		} // end for GetDatasetCount()

		status = 1;
		errCode = "";
		errMsg = "";
		
		//logLevel = Trade::TradeLog::INFO_LEVEL;

	}
	else if(nRet == 1)
	{
		IF2UnPacker *lpUnPacker = (IF2UnPacker *)Pointer;
		errMsg = lpUnPacker->GetStr("error_info");
		GenResponse(boost::lexical_cast<int>(lpUnPacker->GetStr("error_no")), errMsg, response, status, errCode, errMsg);
		
		bRet = true;
		goto FINISH;
	}
	else if(nRet == 2)
	{
		errMsg = (char*)Pointer;
		
		GenResponse(nRet, errMsg, response, status, errCode, errMsg);
		bRet = true;
		goto FINISH;
	}
	else if (nRet == 3)
	{
		GenResponse(nRet, "ҵ������ʧ��", response, status, errCode, errMsg);
		bRet = true;
		goto FINISH;
	}
	else
	{
		
		GenResponse(nRet, lpConnection->GetErrorMsg(nRet), response, status, errCode, errMsg);
		bRet = true;
		goto FINISH;
	}


FINISH:
	

	return bRet;
}

