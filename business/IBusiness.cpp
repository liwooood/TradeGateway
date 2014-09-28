#include "stdafx.h"

#include "IBusiness.h"

#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "./config/ConfigManager.h"
#include "./output/FileLog.h"
#include "./connectpool/ConnectManager.h"
#include "./encrypt/MyBotan.h"


IBusiness::IBusiness(void)
{
	m_bConnected = false;

	SOH = "\x01";
}


IBusiness::~IBusiness(void)
{
}

void IBusiness::SetCounter(Counter * counter)
{
	this->m_Counter = counter;
}

Counter* IBusiness::GetCounter()
{
	return m_Counter;
}

bool IBusiness::IsConnected()
{
	return m_bConnected;
}



bool IBusiness::ParseRequest(std::string& request)
{
	std::vector<std::string> keyvalues;
	std::string keyvalue = "";
	std::vector<std::string> kv;
	std::string key = "";
	std::string value = "";


	reqmap.clear();

	
	boost::split(keyvalues, request, boost::is_any_of(SOH)); // ע����Ҫͨ�������ļ�����

	for (std::vector<std::string>::iterator it = keyvalues.begin(); it != keyvalues.end(); it++)
	{
		keyvalue = *it;
		

		if (keyvalue.empty())
		{
			return false;
		}
		
		kv.clear();
		boost::split(kv, keyvalue, boost::is_any_of("="));

		if (kv.size() != 2)
		{
			return false;
		}

		key = kv[0];
		value = kv[1];

		reqmap[key] = value;
		
	}

	
	sysNo = reqmap["cssweb_sysNo"];

	bt = reqmap["cssweb_busiType"];
	busiType = boost::lexical_cast<int>(bt);

	sysVer = reqmap["cssweb_sysVer"];
	funcid = reqmap["cssweb_funcid"];
	route = reqmap["cssweb_route"];
	hardinfo = reqmap["cssweb_hardinfo"];
	note = reqmap["cssweb_note"];
	cssweb_cacheFlag = reqmap["cssweb_cacheFlag"];


	branchId = reqmap["branch_no"]; // Ӫҵ��id
	account = reqmap["client_id"]; // �ͻ���

	return true;
}

bool IBusiness::FilterRequestField(std::string& key)
{
	if (key == "cssweb_sysNo")
	{
		return true;
	}
	else if (key == "cssweb_busiType")
	{
		return true;
	}
	else if (key == "cssweb_sysVer")
	{
		return true;
	}
	else if (key == "cssweb_funcid")
	{
		return true;
	}
	else if (key == "cssweb_route")
	{
		return true;
	}
	else if (key == "cssweb_hardinfo")
	{
		return true;
	}
	else if (key == "cssweb_note")
	{
		return true;
	}
	else if (key == "cssweb_cacheFlag")
	{
		return true;
	}
	
	else
	{
		return false;
	}
}

// ��������ɹ��������ݷ�����Ϣ
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
	response += "����ִ�гɹ�����̨ϵͳû�����ݷ��ء�";
	response += SOH;

}

// ���ɴ�����Ϣ
void IBusiness::GenResponse(int nErrCode, std::string sErrMsg, std::string& response, int& status, std::string& errCode, std::string& errMsg)
{
	status = 0;

	errCode = boost::lexical_cast<std::string>(nErrCode);
	errMsg = sErrMsg;
}

