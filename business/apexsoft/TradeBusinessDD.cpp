#include "StdAfx.h"

#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string.hpp>

#include "fiddef.h"
#include "fixapi.h"

#include "TradeBusinessDD.h"
#include "ConfigManager.h"

#include "FileLog.h"
#include "CacheData.h"
#include "errcode.h"


TradeBusinessDD::TradeBusinessDD()
{
	m_hHandle = -1;
}

TradeBusinessDD::TradeBusinessDD(int connId, Counter counter)
{
	m_hHandle = -1;

	this->connId = connId;
	this->counter = counter;
	logFile = "柜台\\顶点";
	connectInfo = "连接序号" + boost::lexical_cast<std::string>(connId) + ", 柜台地址" + counter.serverAddr;
}

TradeBusinessDD::~TradeBusinessDD(void)
{
}


bool TradeBusinessDD::Send(std::string& request, std::string& response, int& status, std::string& errCode, std::string& errMsg)
{
	bool bRet = true;

	std::map<std::string, long> mReturnField;
	int nRet = FALSE;
	std::string fbdm;
	std::string khh;
	long session = 0;

	std::string fid_code = "";
	std::string fid_message = "";
	std::string tmp;


	

	ParseRequest(request);

	// 针对客户端版本更新的特殊处理
	if (funcid.compare("190101") == 0 && sysVer.compare("aphone") == 0)
	{
		std::string ver = "";
		ver = reqmap["cssweb_appversion"];
		if (ver.length() == 0 || ver.compare("") == 0 )
		{
			response = "1";
			response += SOH;
			response += "3";
			response += SOH;

			response += "cssweb_code";
			response += SOH;
			response += "cssweb_msg";
			response += SOH;
			response += "cssweb_gwInfo";
			response += SOH;

			response += "0";
			response += SOH;
			response += gVersionControl::instance().forceupdatemsg;
			response += SOH;
			response += "0.0.0.0:5000";
			response += SOH;

			status = 1;
			errCode = "";
			errMsg = "";
			goto FINISH;
		}

		if ( gVersionControl::instance().mapVersions.find(ver) != gVersionControl::instance().mapVersions.end())
		{
			std::string update = gVersionControl::instance().mapVersions[ver];
			if (update.compare("force") == 0)
			{
				response = "1";
				response += SOH;
				response += "3";
				response += SOH;

				response += "cssweb_code";
				response += SOH;
				response += "cssweb_msg";
				response += SOH;
				response += "cssweb_gwInfo";
				response += SOH;

				response += "0";
				response += SOH;
				response += gVersionControl::instance().forceupdatemsg;
				response += SOH;
				response += "0.0.0.0:5000";
				response += SOH;

				status = 1;
				errCode = "";
				errMsg = "";

				goto FINISH;
			}
		}
		else
		{
			response = "1";
			response += SOH;
			response += "3";
			response += SOH;

			response += "cssweb_code";
			response += SOH;
			response += "cssweb_msg";
			response += SOH;
			response += "cssweb_gwInfo";
			response += SOH;

			response += "0";
			response += SOH;
			response += gVersionControl::instance().forceupdatemsg;
			response += SOH;
			response += "0.0.0.0:5000";
			response += SOH;

			status = 1;
			errCode = "";
			errMsg = "";

			goto FINISH;
		}
	}

	// 南京证券禁止修改资料功能
	if ( (sysNo.compare("njzq_jlp") == 0 || sysNo.compare("njzq_flash") == 0) && funcid.compare("202002") == 0)
	{
		GenResponse(NO_ACCESS_ERROR, gError::instance().GetErrMsg(NO_ACCESS_ERROR), response, status, errCode, errMsg);
		goto FINISH;
	}
	
	// 应该把功能封装进cache
	if (!cssweb_cacheFlag.empty())
	{
		if (g_CacheData.m_mapCacheData.find(cssweb_cacheFlag) != g_CacheData.m_mapCacheData.end())
		{
			std::string value = g_CacheData.m_mapCacheData[cssweb_cacheFlag];
		
			if (!value.empty())
			{
				response = value;

				status = 1;
				errCode = "";
				errMsg = "";
				//logLevel = Trade::TradeLog::INFO_LEVEL;
				gFileLog::instance().debug(logFile, "从缓存获取" + cssweb_cacheFlag);
				
				goto FINISH;
			}
		}
	}
	

	// 判断功能号是否已定义
	if (g_DingDian.m_mReturnField.find(funcid) == g_DingDian.m_mReturnField.end())
	{
		GenResponse(FUNC_NOT_FOUND_ERROR, gError::instance().GetErrMsg(FUNC_NOT_FOUND_ERROR), response, status, errCode, errMsg);
		goto FINISH;
	}

	
	
	
	session = Fix_AllocateSession(m_hHandle);
	
	// set gydm
	Fix_SetGYDM(session, counter.m_sGydm.c_str());
	

	// set fbdm & dest fbdm
	khh = reqmap["FID_KHH"];
	if (!khh.empty())
	{
		 fbdm = khh.substr(0,4);
	}
	else
	{
		std::string zjzh = reqmap["FID_ZJZH"];
		fbdm = zjzh.substr(0,4);
	}

	Fix_SetFBDM(session, fbdm.c_str());
	Fix_SetDestFBDM(session, fbdm.c_str());



	// set wtfs
	Fix_SetWTFS(session, counter.entrustMode.c_str());
	


	// set node
	if (sysVer.compare("iphone")==0 || sysVer.compare("aphone")==0)
	{
		std::string node;
		node =reqmap["cssweb_note"];

		//前端格式18621112013
		if (node.find("ZRSJ") == std::string::npos)
			node = "ZRSJ:" + node;

	
		Fix_SetNode(session, node.c_str());
	}
	else
	{
		std::string hardinfo;
		hardinfo = reqmap["cssweb_hardinfo"];

		std::string ip;
		ip = reqmap["cssweb_note"];

		std::string mac = "";
		std::string diskid = "";

		GetMacAndDiskID(hardinfo, mac, diskid);

		//ZR:IP:MAC:DiskID
		std::string node = "ZR:" + ip + ":" + mac + ":" + diskid;

		Fix_SetNode(session, node.c_str());
	}


	
	Fix_CreateHead(session, boost::lexical_cast<long>(funcid));

	
	for (std::vector<std::string>::iterator it = keys.begin(); it != keys.end(); it++)
	{
		std::string key = *it;
		std::string value = reqmap[key];

		
		
		if (key.compare("FBDM")==0)
		{
			Fix_SetFBDM(session, value.c_str());
			Fix_SetDestFBDM(session, value.c_str());
		}
		else if(key.compare("FID_JYMM")==0 || key.compare("FID_MM")==0 || key.compare("FID_NEWMM")==0 || key.compare("FID_ZJMM")==0 || key.compare("FID_WBZHMM")==0)
		{
			char szPwd[256];
			memset(szPwd, 0x00, sizeof(szPwd));

			EncryptPwd(value.c_str(), szPwd);
			if (szPwd != NULL)
			{
				long l = g_DingDian.m_mRequestField[key];
				Fix_SetItem(session, l, szPwd);
			}
		}
		else
		{
			long l = g_DingDian.m_mRequestField[key];
			Fix_SetItem(session, l, value.c_str());
		}
	} // end for
	


	// 默认为30秒
	Fix_SetTimeOut(session, counter.m_nRecvTimeout);

	nRet = Fix_Run(session);
	if (!nRet)
	{
		long lErrCode = Fix_GetCode(session);
		char szErrMsg[1024];
		Fix_GetErrMsg(session, szErrMsg, sizeof(szErrMsg));

		GenResponse(lErrCode, szErrMsg, response, status, errCode, errMsg);

		goto FINISH;
	}	



	char szValue[512];

	memset(szValue, '\0', sizeof(szValue));
	Fix_GetItem(session, FID_CODE, szValue, sizeof(szValue), 0);
	fid_code = szValue;
	

	memset(szValue, '\0', sizeof(szValue));
	Fix_GetItem(session, FID_MESSAGE, szValue, sizeof(szValue), 0);
	fid_message = szValue;
	
	

	int fidcode = -1;
	if (!fid_code.empty())
	{
		fidcode = boost::lexical_cast<int>(fid_code);
	}

	if (fid_code.empty() || fidcode >= 0)
	{
		
		long nRows = Fix_GetCount(session);
		

		// 处理成功，无数据返回
		if (nRows == 0)
		{
			RetNoRecordRes(response, status);
			
			goto FINISH;
		}

		//gFileLog::instance().Log("开始处理结果集");
		mReturnField = g_DingDian.m_mReturnField[funcid];
		int nCols = mReturnField.size();

	

		response = boost::lexical_cast<std::string>(nRows);
		response += SOH;
		response += boost::lexical_cast<std::string>(nCols);
		response += SOH;

		


		// 列头
		for (std::map<std::string, long>::iterator it = mReturnField.begin(); it != mReturnField.end(); it++)
		{
			std::string sColName = it->first;

			response += sColName;
			response += SOH;
		}

		


		for (int row=0; row<nRows; row++)
		{
			char szValue[512];
			for (std::map<std::string, long>::iterator it = mReturnField.begin(); it != mReturnField.end(); it++)
			{
				std::string sKey = it->first;
				long lKey = it->second;

			
				memset(szValue, '\0', sizeof(szValue));
				Fix_GetItem(session, lKey, szValue, sizeof(szValue), row);

				response += szValue;
				response += SOH;
			} // end for col
		} // end for row

	

		status = 1;
		errCode = "";
		errMsg = "";
		//logLevel = Trade::TradeLog::INFO_LEVEL;

		// 结果集缓存
		if (!cssweb_cacheFlag.empty())
		{
			gFileLog::instance().debug(logFile, "加入缓存" + cssweb_cacheFlag);
			g_CacheData.m_mapCacheData[cssweb_cacheFlag] = response;
			
		}
	}
	else
	{
		GenResponse(fidcode, fid_message, response, status, errCode, errMsg);
		goto FINISH;
	}


FINISH:
	
	if (session != 0)
	{
		Fix_ReleaseSession(session);
	}

	return bRet;

}


char* TradeBusinessDD::EncryptPwd(const char* plainText, char* szPwd)
{

	if (plainText == NULL)
		return NULL;

	strcpy(szPwd, plainText);
	Fix_Encode(szPwd);
	return szPwd;
}


bool TradeBusinessDD::CreateConnect()
{
	int nRet = 0;


	std::string gtAddr = counter.m_sIP + "@" + boost::lexical_cast<std::string>(counter.m_nPort) + "/tcp";
	m_hHandle = Fix_Connect(gtAddr.c_str(), counter.m_sUserName.c_str(), counter.m_sPassword.c_str(), counter.m_nConnectTimeout); 

		
	if (m_hHandle == 0)
	{
		gFileLog::instance().error(logFile, "建立连接失败" + connectInfo);

		return false;
	}
	else
	{
		gFileLog::instance().debug(logFile, "建立连接成功" + connectInfo);
		return true;
	}
}

void TradeBusinessDD::CloseConnect()
{
	if (m_hHandle != -1)
	{
		Fix_Close(m_hHandle);
		m_hHandle = -1;
	}

	gFileLog::instance().debug(logFile, "关闭连接" + connectInfo);
}

void TradeBusinessDD::GetMacAndDiskID(std::string hardinfo, std::string& mac, std::string& diskid)
{
	std::vector<std::string> info;
	boost::split(info, hardinfo, boost::is_any_of("|")); // 注意需要通过配置文件配置
	int size = info.size();
	
	if (size < 2)
		return;

	mac = info[0];

	diskid = info[1];
}
