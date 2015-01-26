#include "StdAfx.h"


#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string.hpp>

#include "KDEncodeCli.h"
#include "kcbpcli.h"

#include "TradeBusinessJZ.h"
//#include "LogManager.h"
#include "ConnectManager.h"
#include "ConfigManager.h"
#include "Dogskin.h"
//ICU
//#include <unicode/putil.h>
//#include <unicode/ucnv.h>


#include "FileLog.h"


TradeBusinessJZ::TradeBusinessJZ()
{
	handle = NULL;
}

TradeBusinessJZ::TradeBusinessJZ(int connId, Counter counter)
{
	handle = NULL;
	this->connId = connId;
	this->counter = counter;
	
	logFile = "柜台\\金证";
	connectInfo = "连接序号" + boost::lexical_cast<std::string>(connId) + ", 柜台地址" + counter.serverAddr;
}

TradeBusinessJZ::~TradeBusinessJZ(void)
{
}


bool TradeBusinessJZ::CreateConnect()
{
	int nRet = 0;


	tagKCBPConnectOption stKCBPConnection;
	memset(&stKCBPConnection, 0x00, sizeof(stKCBPConnection));

	strcpy(stKCBPConnection.szServerName, counter.m_sServerName.c_str());
	stKCBPConnection.nProtocal = 0;
	strcpy(stKCBPConnection.szAddress, counter.m_sIP.c_str());
	stKCBPConnection.nPort = counter.m_nPort;
	strcpy(stKCBPConnection.szSendQName, counter.m_sReq.c_str());
	strcpy(stKCBPConnection.szReceiveQName, counter.m_sRes.c_str());

	nRet = KCBPCLI_Init( &handle );
	nRet = KCBPCLI_SetConnectOption( handle, stKCBPConnection );		

	//设置超时
	nRet = KCBPCLI_SetCliTimeOut( handle, counter.m_nConnectTimeout);

	// 设置是否输出调试信息
	//nRet = KCBPCLI_SetOptions( handle, KCBP_OPTION_TRACE, &gConfigManager::instance().m_nIsTradeServerDebug, sizeof(int));

	nRet = KCBPCLI_SQLConnect( handle, stKCBPConnection.szServerName, (char*)counter.m_sUserName.c_str(), (char*)counter.m_sPassword.c_str());
	if (nRet != 0)
	{
		gFileLog::instance().error(logFile, "连接柜台失败" + connectInfo);
		return false;
	}
	else
	{
		gFileLog::instance().debug(logFile, "连接柜台成功" + connectInfo);

		return true;
	}	
}

void TradeBusinessJZ::CloseConnect()
{
	if (handle != NULL)
	{
		KCBPCLI_SQLDisconnect(handle);
		KCBPCLI_Exit(handle);
		handle = NULL;

		gFileLog::instance().debug(logFile, "关闭柜台" + connectInfo);
	}
}

bool TradeBusinessJZ::Send(std::string& request, std::string& response, int& status, std::string& errCode, std::string& errMsg)
{
	bool bRet = true;
	int nRet = 0;
	
	std::string pkgType = "2";
	std::string log;
	std::string content = "";
	int nRows = 0;
	char szErrMsg[512] = {0};



	ParseRequest(request);


	

	nRet = KCBPCLI_BeginWrite(handle);

	KCBPCLI_SetSystemParam(handle, KCBP_PARAM_SERVICENAME, (char*)funcid.c_str());
	KCBPCLI_SetSystemParam(handle, KCBP_PARAM_RESERVED, (char*)reqmap["orgid"].c_str());
	KCBPCLI_SetSystemParam(handle, KCBP_PARAM_PACKETTYPE, (char*)pkgType.c_str());

	for (std::vector<std::string>::iterator it = keys.begin(); it != keys.end(); it++)
	{
		std::string key = *it;
		std::string value = reqmap[key];

		
		if(key.compare("trdpwd") == 0)
		{
			std::string encrypt_pwd = g_Encrypt.EncryptPWD(value);
			KCBPCLI_SetValue(handle, (char*)key.c_str(), (char*)encrypt_pwd.c_str());
		}
		else if (key.compare("newpwd") == 0)
		{
			std::string encrypt_pwd = g_Encrypt.EncryptPWD(value);
			KCBPCLI_SetValue(handle, (char*)key.c_str(), (char*)encrypt_pwd.c_str());	
		}
		else if(key.compare("fundpwd")==0 || key.compare("newfundpwd")==0 || key.compare("oldfundpwd")==0 )
		{
			std::string encrypt_pwd = g_Encrypt.EncryptPWD(value);
			KCBPCLI_SetValue(handle, (char*)key.c_str(), (char*)encrypt_pwd.c_str());			
		}
		else
		{
			KCBPCLI_SetValue(handle, (char*)key.c_str(), (char*)value.c_str());
		}
	} // end for



	nRet  = KCBPCLI_SQLExecute(handle, (char*)funcid.c_str()); // 功能号
	if (nRet != 0)
	{
		this->GenResponse(nRet, "KCBPCLI_SQLExecute error", response, status, errCode, errMsg);
		goto FINISH;
	}


	int nErrCode = 0;
	nRet = KCBPCLI_GetErrorCode(handle, &nErrCode);
	if (nErrCode != 0)
	{
		KCBPCLI_GetErrorMsg(handle, szErrMsg );
		
		this->GenResponse(nErrCode, szErrMsg, response, status, errCode, errMsg);
		goto FINISH;
	}


	nRet = KCBPCLI_RsOpen(handle);
	if (nRet != 0 && nRet != 100)
	{
		
		this->GenResponse(nRet, "KCBPCLI_RsOpen error", response, status, errCode, errMsg);
		goto FINISH;
	}


	nRet = KCBPCLI_SQLFetch(handle);

	char szTmpbuf[1024];
	memset(szTmpbuf, 0x00, sizeof(szTmpbuf));
	nRet = KCBPCLI_RsGetColByName(handle, "CODE", szTmpbuf );
	if (nRet != 0)
	{
		
		this->GenResponse(nRet, "KCBPCLI_RsGetColByName error", response, status, errCode, errMsg);
		goto FINISH;
	}

	if (strcmp(szTmpbuf, "0") != 0)
	{
		errCode = szTmpbuf;

		KCBPCLI_RsGetColByName(handle, "MSG", szTmpbuf );
		errMsg = szTmpbuf;

		
		this->GenResponse(boost::lexical_cast<int>(errCode), errMsg, response, status, errCode, errMsg);
		goto FINISH;
	}

	
	nRet = KCBPCLI_SQLMoreResults(handle);
	if (nRet != 0)
	{
		int nErrCode = 0;
		KCBPCLI_GetErrorCode(handle, &nErrCode);
		KCBPCLI_GetErrorMsg(handle, szErrMsg );

		if (nErrCode == 0)
		{
			RetNoRecordRes(response, status);
			goto FINISH;
		}


		
		this->GenResponse(nErrCode, szErrMsg, response, status, errCode, errMsg);
		goto FINISH;

		// 是不是执行成功，没有数据返回
	}
	

/*
	if (nRet != 0 )
	{
		RetErrRes(Cssweb::CsswebMessage::ERROR_LEVEL, response, response_pb, boost::lexical_cast<std::string>(nRet), "KCBPCLI_SQLMoreResults error");
		return;
	}
*/



	//nRet = KCBPCLI_SQLNumResultCols(m_pConn->handle, &nCol);
	

	
	nRows = 0;

	int nCols = 0;
	KCBPCLI_RsGetColNum(handle, &nCols);


	bool bGetColName = true;
	
				
	while( !KCBPCLI_RsFetchRow(handle) )
	{
		nRows += 1;

		// 取列头
		if (bGetColName)
		{
			for( int i = 1; i <= nCols; i++ )
			{
				char szColName[256];
				memset(szColName, 0x00, sizeof(szColName));

				KCBPCLI_RsGetColName( handle, i, szColName, sizeof(szColName)-1 );

				content += szColName;
				content += SOH;
			}

			bGetColName = false;
		} // end for col

		//处理行数据
		for( int i = 1; i <= nCols; i++ )
		{
			//获取列名
			char szColName[256];
			memset(szColName, 0x00, sizeof(szColName));
			KCBPCLI_RsGetColName( handle, i, szColName, sizeof(szColName)-1 );

			//根据列名获取列值
			char szColValue[1024];
			KCBPCLI_RsGetColByName( handle, szColName, szColValue );

			content += szColValue;
			content += SOH;
		} // end for row
	} // end while

	

	if (nRows == 0)
	{
		// 执行成功，柜台没有数据返回
		RetNoRecordRes(response, status);
		goto FINISH;
	}

	response = boost::lexical_cast<std::string>(nRows);
	response += SOH;
	response += boost::lexical_cast<std::string>(nCols);
	response += SOH;

	response += content;


	nRet = KCBPCLI_SQLCloseCursor(handle);


	status = 1;
	errCode = "";
	errMsg = "";
	
	

FINISH:
	return bRet;
}
