#include "StdAfx.h"




#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>


#include "TradeBusinessT2.h"

#include "ConnectManager.h"

#include "ConfigManager.h"


// ICU
//#include <unicode/putil.h>
//#include <unicode/ucnv.h>

#include "mybotan.h"
#include "FileLog.h"
#include "errcode.h"
#include "FileLogManager.h"



TradeBusinessT2::TradeBusinessT2()
{
	lpConfig = NULL;
	lpConnection = NULL;
}

TradeBusinessT2::TradeBusinessT2(int ConnectNo, Counter counter)
{
	lpConnection = NULL;
	lpConfig = NULL;

	m_nID = ConnectNo;
	m_Counter = counter;

	boost::format fmt("序号%1% 柜台地址%2%:%3%");
	fmt % m_nID % m_Counter.m_sIP % m_Counter.m_nPort;
	m_sServerInfo = fmt.str();

	connectTimeout = gConfigManager::instance().m_nConnectPoolConnectTimeout * 1000;
	readWriteTimeout = gConfigManager::instance().m_nConnectPoolReadWriteTimeout * 1000;
}



bool TradeBusinessT2::CreateConnect()
{
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


	lpConnection = NewConnection(lpConfig);
	lpConnection->AddRef();

	nRet = lpConnection->Create(NULL);

		
	nRet = lpConnection->Connect(connectTimeout);

	if (nRet != 0)
	{
		std::string sErrMsg = lpConnection->GetErrorMsg(nRet);
			std::string msg = "建立连接失败" + s + ",错误信息" + sErrMsg;
			gFileLog::instance().Log(msg, 0, "恒生");

		m_bConnected = false;
		return false;
	}
	else
	{
		m_bConnected = true;
		return true;
	}
}

void TradeBusinessT2::CloseConnect()
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
}

/*
只有网络通信错误才返回false
业务错误也返回true
*/
bool TradeBusinessT2::Send(std::string& request, std::string& response, int& status, std::string& errCode, std::string& errMsg)
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

			GenResponse(PARAM_ERROR, gError::instance().GetErrMsg(PARAM_ERROR), response, status, errCode, errMsg);

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

		GenResponse(PARAM_ERROR, gError::instance().GetErrMsg(PARAM_ERROR), response, status, errCode, errMsg);
		bRet = true;	
		goto FINISH;
	}



	IF2Packer* pack = NewPacker(2);
	pack->AddRef();

	pack->BeginPack();

	// 设置key
	for (std::vector<std::string>::iterator it = keys.begin(); it != keys.end(); it++)
	{
		std::string key = *it;

		//pack->AddField(key.c_str());
		pack->AddField(key.c_str(), 'S', 8000);
	}

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

			pack->AddStr(value.c_str());
			
		}
	}

	pack->EndPack();

	

	
	
	//nRet = m_pConn->lpConnection->SendBiz(boost::lexical_cast<int>(funcid), pack, 0, boost::lexical_cast<int>(system)); //  同步
	/*
	GenResponse(0, "临时退出", response, status, errCode, errMsg);
		bRet = true;
		goto FINISH;
		*/

	if (nRoute == -1)
		nRet = lpConnection->SendBiz(lFuncId, pack); //  同步
	else
		nRet = lpConnection->SendBiz(lFuncId, pack, 0, nRoute); //  同步

	if (nRet < 0)
	{
		pack->Release();

		
		GenResponse(nRet, lpConnection->GetErrorMsg(nRet), response, status, errCode, errMsg);
		bRet = true;
		goto FINISH;
	}


	pack->FreeMem(pack->GetPackBuf());
	pack->Release();





	// 接收数据
	void *Pointer = NULL;

	//nRet = m_pConn->lpConnection->RecvBiz(nRet, &Pointer, gConfigManager::instance().m_nConnectTimeout, 0);
	
	
	nRet = lpConnection->RecvBiz(nRet, &Pointer, m_Counter.m_nRecvTimeout*1000);


	// 返回成功
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
			// 不应该有结果集返回
			if (!it->second.hasResultRet)
			{
				gFileLog::instance().Log(funcid + "不应该返回结果集");

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
			// 设置当前结果集
			lpUnPacker->SetCurrentDatasetByIndex(i);

			for (int c = 0; c < nCols; c++)
			{
				std::string sColName = lpUnPacker->GetColName(c);
					
				response += sColName;
					
				response += SOH;
			} // end all column

			// 打印所有记录
			for (int r = 0; r < nRows; r++)
			{
				// 打印每条记录
				for (int c = 0; c < nCols; c++)
				{
					std::string temp = lpUnPacker->GetStrByIndex(c);

					// 国泰君安一户通特殊处理
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
		GenResponse(nRet, "业务包解包失败", response, status, errCode, errMsg);
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

