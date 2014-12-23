#include "stdafx.h"

#include "IBusiness.h"

#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "ConfigManager.h"
#include "FileLog.h"
#include "ConnectManager.h"
#include "MyBotan.h"


IBusiness::IBusiness(void)
{
	m_bConnected = false;

	SOH = "\x01";
}




Counter& IBusiness::GetCounter()
{
	return counter;
}


std::string IBusiness::GetConnectInfo()
{
	return connectInfo;
}

bool IBusiness::ParseRequest(std::string& request)
{
	std::vector<std::string> keyvalues;
	//std::string keyvalue = "";
	std::vector<std::string> kv;
	std::string key = "";
	std::string value = "";


	reqmap.clear();
	keys.clear();
	

	//req = boost::to_lower_copy(request);
	req = request;
	
	boost::split(keyvalues, req, boost::is_any_of(SOH)); // 注意需要通过配置文件配置

	int size = keyvalues.size();
	//std::string si = boost::lexical_cast<std::string>(size);
	//gFileLog::instance().Log("size=" + si);

	for (std::vector<std::string>::iterator it = keyvalues.begin(); it != keyvalues.end(); it++)
	{
		std::string  keyvalue = *it;
		

		if (keyvalue.empty())
		{
			//gFileLog::instance().Log("keyvalue.empty()");

			continue;
		}
		
		std::size_t found = keyvalue.find_first_of("=");
		

		if (found != std::string::npos)
		{
			std::string k = keyvalue.substr(0, found);
			key = boost::to_lower_copy(k);

			value = keyvalue.substr(found + 1);

			reqmap[key] = value;
		}

		/*
		kv.clear();
		boost::split(kv, keyvalue, boost::is_any_of("="));

		if (kv.size() < 2)
		{
			//gFileLog::instance().Log("kv.size()");
			return false;
		}

		key = kv[0];
		value = kv[1];
		*/
		
		
	}

	
	sysNo = reqmap["cssweb_sysno"];

	bt = reqmap["cssweb_busitype"];
	busiType = boost::lexical_cast<int>(bt);

	sysVer = reqmap["cssweb_sysver"];

	funcid = reqmap["cssweb_funcid"];
	

	route = reqmap["cssweb_route"];
	hardinfo = reqmap["cssweb_hardinfo"];
	note = reqmap["cssweb_note"];
	cssweb_cacheFlag = reqmap["cssweb_cacheflag"];


	branchId = reqmap["branch_no"]; // 营业部id
	//account = reqmap["client_id"]; // 客户号
	account = reqmap["cssweb_account"];

	// 得到请求数
	std::map<std::string, std::string>::iterator itFound = reqmap.find("cssweb_num");
	if (itFound != reqmap.end())
	{
		num = boost::lexical_cast<int>(itFound->second);
	}
	else
	{
		num = 1;
	}


	for (std::map<std::string, std::string>::iterator it = reqmap.begin(); it != reqmap.end(); it++)
	{
		key = it->first;
		
		// 过滤自定义参数
		std::size_t found = key.find("cssweb_");
		if (found != std::string::npos)
			continue;

		if (num == 1)
		{
			keys.push_back(key);
		}
		else
		{
			// a=data, a_1=data, a_2=data, 只提取a， 
			bool bFound = false;
			for (int i=1; i<num; i++)
			{
				std::string newkey = key + "_" + boost::lexical_cast<std::string>(i);
				std::map<std::string, std::string>::iterator itFound = reqmap.find(newkey);
				if (itFound != reqmap.end())
				{
					//TRACE("k=");
					//TRACE(key.c_str());
					//TRACE("\n");

					bFound = true;
					break;
				}
			}
		
			if (bFound)
			{
				keys.push_back(key);

				//TRACE("提取key=");
				//TRACE(key.c_str());
				//TRACE("\n");
			}
		}
	}

	

	return true;
}

// 生成请求成功，无数据返回信息
void IBusiness::RetNoRecordRes(std::string& response, int& status)
{
	status = 1;

	response = "1";
	response += SOH;
	response += "2";
	response += SOH;

	response += "cssweb_code";
	response += SOH;
	response += "cssweb_msg";
	response += SOH;

	response += "1";
	response += SOH;
	response += "请求执行成功，柜台系统没有数据返回。";
	response += SOH;

}

// 生成错误信息
void IBusiness::GenResponse(int nErrCode, std::string sErrMsg, std::string& response, int& status, std::string& errCode, std::string& errMsg)
{
	status = 0;
	errCode = boost::lexical_cast<std::string>(nErrCode);
	errMsg = sErrMsg;

	response = "1";
	response += SOH;
	response += "4";
	response += SOH;

	response += "cssweb_code";
	response += SOH;
	response += "cssweb_msg";
	response += SOH;
	response += "cssweb_gwinfo";
	response += SOH;
	response += "cssweb_counter";
	response += SOH;

	response += errCode;
	response += SOH;
	response += errMsg;
	response += SOH;

	response += gatewayInfo;
	response += SOH;
	response += counter.serverAddr;
	response += SOH;
	
}

void IBusiness::SetGatewayInfo(std::string gatewayIP, std::string gatewayPort)
{
	gatewayInfo = gatewayIP + ":" + gatewayPort;
}
