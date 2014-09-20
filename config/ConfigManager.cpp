#include "stdafx.h"




#include <list>
#include <iostream>
#include <fstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string.hpp>

#include "./lib/xml/pugixml.hpp"

#include "ConfigManager.h"
#include "./connectpool/counter.h"
#include "./connectpool/ConnectManager.h"





using namespace boost::filesystem;




ConfigManager::ConfigManager(void)
{
}

ConfigManager::~ConfigManager(void)
{
}

bool ConfigManager::LoadConfig(std::string sPath)
{
	if (sPath.empty())
	{
		char szPath[255];
		memset(szPath, 0x00, sizeof(szPath));
		::GetModuleFileName(NULL, szPath, sizeof(szPath));
		m_sExeFile = szPath;

		boost::filesystem::path p(szPath);

	
		m_sPath = p.parent_path().string();
	}
	else
	{
		m_sPath = sPath;
	}

	std::string sConfigFile = m_sPath + "\\tradegateway.xml";
	



/*

	boost::format fmt("%1%:%2%");
	fmt % m_sIPAddr % m_nTcpPort;
	m_sWebTradeAddr = fmt.str();


*/

	



	pugi::xml_document doc;
	if (!doc.load_file(sConfigFile.c_str()))
		return false;

	pugi::xpath_node node;
	std::string node_path;

	pugi::xpath_node_set nodes;

	std::string node_value;

	// ͨ�Ų�
	node = doc.select_single_node("/config/communication/ip");
	m_sIp = node.node().child_value();

	node = doc.select_single_node("/config/communication/ssl/port");
	m_nSslPort = boost::lexical_cast<int>(node.node().child_value());
	node = doc.select_single_node("/config/communication/ssl/workerthreadpool");
	m_nSslWorkerThreadPool = boost::lexical_cast<int>(node.node().child_value());
	node = doc.select_single_node("/config/communication/ssl/sendthreadpool");
	m_nSslSendThreadPool = boost::lexical_cast<int>(node.node().child_value());
	node = doc.select_single_node("/config/communication/ssl/auth");
	m_nAuth = boost::lexical_cast<int>(node.node().child_value());
	node = doc.select_single_node("/config/communication/ssl/enable");
	m_nSslEnable = boost::lexical_cast<int>(node.node().child_value());

	

	node = doc.select_single_node("/config/communication/tcp/port");
	m_nTcpPort = boost::lexical_cast<int>(node.node().child_value());
	node = doc.select_single_node("/config/communication/tcp/workerthreadpool");
	m_nTcpWorkerThreadPool = boost::lexical_cast<int>(node.node().child_value());
	node = doc.select_single_node("/config/communication/tcp/sendthreadpool");
	m_nTcpSendThreadPool = boost::lexical_cast<int>(node.node().child_value());
	node = doc.select_single_node("/config/communication/tcp/enable");
	m_nTcpEnable = boost::lexical_cast<int>(node.node().child_value());


	node = doc.select_single_node("/config/communication/ssl_new/port");
	m_nSslNewPort = boost::lexical_cast<int>(node.node().child_value());
	node = doc.select_single_node("/config/communication/ssl_new/workerthreadpool");
	m_nSslNewWorkerThreadPool = boost::lexical_cast<int>(node.node().child_value());
	node = doc.select_single_node("/config/communication/ssl_new/sendthreadpool");
	m_nSslNewSendThreadPool = boost::lexical_cast<int>(node.node().child_value());
	node = doc.select_single_node("/config/communication/ssl_new/auth");
	m_nNewAuth = boost::lexical_cast<int>(node.node().child_value());
	node = doc.select_single_node("/config/communication/ssl_new/enable");
	m_nSslNewEnable = boost::lexical_cast<int>(node.node().child_value());


	node = doc.select_single_node("/config/communication/tcp_new/port");
	m_nTcpNewPort = boost::lexical_cast<int>(node.node().child_value());
	node = doc.select_single_node("/config/communication/tcp_new/workerthreadpool");
	m_nTcpNewWorkerThreadPool = boost::lexical_cast<int>(node.node().child_value());
	node = doc.select_single_node("/config/communication/tcp_new/sendthreadpool");
	m_nTcpNewSendThreadPool = boost::lexical_cast<int>(node.node().child_value());
	node = doc.select_single_node("/config/communication/tcp_new/enable");
	m_nTcpNewEnable = boost::lexical_cast<int>(node.node().child_value());


	node = doc.select_single_node("/config/communication/zlib");
	zlib = boost::lexical_cast<int>(node.node().child_value());

	node = doc.select_single_node("/config/communication/RunMode");
	nRunMode = boost::lexical_cast<int>(node.node().child_value());


	// ��̨����
	node = doc.select_single_node("/config/Counter_Common/connectpool/min");
	m_nConnectPoolMin = boost::lexical_cast<int>(node.node().child_value());
	node = doc.select_single_node("/config/Counter_Common/connectpool/max");
	m_nConnectPoolMax = boost::lexical_cast<int>(node.node().child_value());
	node = doc.select_single_node("/config/Counter_Common/connectpool/increase");
	m_nConnectPoolIncrease = boost::lexical_cast<int>(node.node().child_value());


	

	node = doc.select_single_node("/config/Counter_Common/connect_retry");
	m_nConnectRetry = boost::lexical_cast<int>(node.node().child_value());
	node = doc.select_single_node("/config/Counter_Common/business_retry");
	m_nBusinessRetry = boost::lexical_cast<int>(node.node().child_value());



	pugi::xpath_node_set system_nodes = doc.select_nodes("/config/system");
	int i=0;
	pugi::xpath_node_set::const_iterator it_system;

	for (i=1,  it_system = system_nodes.begin(); it_system != system_nodes.end(); ++it_system, ++i)
	{
		pugi::xpath_node node = *it_system;

		BusinessSystem system;

		std::string sysid = node.node().child_value("id");

		system.id = sysid;

		TRACE("system id = %s\n", sysid.c_str());
		std::string desc = node.node().child_value("desc");

		

		// ��ϵͳ�������е�ҵ������
		//std::map<BUSI_TYPE, std::map<std::string, std::vector<Counter>> > busis; ʹ��BusinessSystem
		boost::format fmt("/config/system[%1%]/BusinessType");
		fmt % i;
		node_path = fmt.str();
		TRACE("��ѯҵ������ = %s\n", node_path.c_str());
		pugi::xpath_node_set busitype_nodes = doc.select_nodes(node_path.c_str());
		
		int j=0;
		pugi::xpath_node_set::const_iterator it_busitype;
		for (j=1, it_busitype = busitype_nodes.begin(); it_busitype != busitype_nodes.end(); ++it_busitype, ++j)
		{
			pugi::xpath_node node = *it_busitype;

			BusinessType busiType;

			std::string type = node.node().child_value("type");
			int BusiType = boost::lexical_cast<int>(type);
			TRACE("ҵ������ id = %d\n", BusiType);

			int CounterType = boost::lexical_cast<int>(node.node().child_value("CounterType"));
			TRACE("��̨���� id = %d\n", CounterType);

			int m_nConnectTimeout = boost::lexical_cast<int>(node.node().child_value("connect_timeout"));
	
			int m_nIdleTimeout = boost::lexical_cast<int>(node.node().child_value("idle_timeout"));
	
			int m_nRecvTimeout = boost::lexical_cast<int>(node.node().child_value("recv_timeout"));

			std::string gydm = node.node().child_value("gydm");
			std::string wtfs_mobile = node.node().child_value("wtfs_mobile");
			std::string wtfs_web = node.node().child_value("wtfs_web");


			// ��ҵ�������������п��õ�Ӫҵ��
			//std::map<std::string, std::vector<Counter>> branchs;
			
			boost::format fmt("/config/system[%1%]/BusinessType[%2%]/branch");
			fmt % i %j;
			node_path = fmt.str();
			TRACE("��ѯӪҵ�� = %s\n", node_path.c_str());
			pugi::xpath_node_set branch_nodes = doc.select_nodes(node_path.c_str());
			pugi::xpath_node_set::const_iterator it_branch;
			int m=0;
			for (m=1, it_branch = branch_nodes.begin(); it_branch != branch_nodes.end(); ++it_branch, ++m)
			{
				pugi::xpath_node node = *it_branch;

				
				int enable = boost::lexical_cast<int>(node.node().child_value("enable"));
				if (enable == 0)
					continue;

				std::string branchList = node.node().child_value("branch_list");
				TRACE("Ӫҵ���Ƿ����� = %s %d\n", branchList.c_str(), enable);

				// ��Ӫҵ���������п��õĹ�̨
				std::vector<Counter> counters;
				boost::format fmt("/config/system[%1%]/BusinessType[%2%]/branch[%3%]/server");
				fmt % i % j % m;
				node_path = fmt.str();
				TRACE("��ѯ��̨ = %s\n", node_path.c_str());
				pugi::xpath_node_set counter_nodes = doc.select_nodes(node_path.c_str());
				pugi::xpath_node_set::const_iterator it_counter;
				for (it_counter = counter_nodes.begin(); it_counter != counter_nodes.end(); ++it_counter)
				{
					pugi::xpath_node node = *it_counter;

					node_value = node.node().child_value("enable");
					int enable = boost::lexical_cast<int>(node_value);
					if (enable == 0)
						continue;

					Counter counter;

					node_value = node.node().child_value("servername");
					
					counter.m_sServerName = node_value;
					

					node_value = node.node().child_value("ip");
					TRACE("��̨ = %s\n", node_value.c_str());
					counter.m_sIP = node_value;

					node_value = node.node().child_value("port");
					counter.m_nPort = boost::lexical_cast<int>(node_value);

					node_value = node.node().child_value("username");
					counter.m_sUserName = node_value;

					node_value = node.node().child_value("password");
					counter.m_sPassword = node_value;

					node_value = node.node().child_value("req");
					counter.m_sReq = node_value;

					node_value = node.node().child_value("res");
					counter.m_sRes = node_value;

					counter.m_eCounterType = ConvertIntToCounterType(CounterType);
					counter.m_nConnectTimeout = m_nConnectTimeout;
					counter.m_nIdleTimeout = m_nIdleTimeout;
					counter.m_nRecvTimeout = m_nRecvTimeout;

					// ����ר��
					
					counter.m_sGydm = gydm;

					// ί�з�ʽ
					
					counter.m_sWtfs_mobile = wtfs_mobile;
					counter.m_sWtfs_web = wtfs_web;

					counters.push_back(counter);///////////////////////////
				} // end for counter

			//	ConnectPool * pool = new ConnectPool(counters);
				//busiType.connPool[branchList] = pool;//////////////////////
				busiType.counterType = ConvertIntToCounterType(CounterType);

				Branch branch;
				branch.servers = counters;

				busiType.branches[branchList] = branch;
			} // end for branch

			
			system.busis[ConvertIntToBusiType(BusiType)] = busiType;/////////////////////////////
		} // end for business type

		g_ConnectManager.systems[sysid] = system;
	} // end for system



	// ��־
	node = doc.select_single_node("/config/log/loglevel");
	m_nLogLevel = boost::lexical_cast<int>(node.node().child_value());
	node = doc.select_single_node("/config/log/response_len");
	m_nResponseLen = boost::lexical_cast<int>(node.node().child_value());

	node = doc.select_single_node("/config/log/filterfuncid");
	m_nFilterFuncId = boost::lexical_cast<int>(node.node().child_value());

	node = doc.select_single_node("/config/log/filterfield");
	m_nFilterField = boost::lexical_cast<int>(node.node().child_value());

	node = doc.select_single_node("/config/log/filtercustid");
	m_sFilterCustId = node.node().child_value();

	node = doc.select_single_node("/config/log/file/enable");
	m_nLogFileEnable = boost::lexical_cast<int>(node.node().child_value());
	node = doc.select_single_node("/config/log/file/path");
	m_sLogFilePath = node.node().child_value();
	node = doc.select_single_node("/config/log/file/threadpool");
	m_nLogFileThreadPool = boost::lexical_cast<int>(node.node().child_value());

	

	node = doc.select_single_node("/config/log/gui/enable");
	m_nLogGuiEnable = boost::lexical_cast<int>(node.node().child_value());
	node = doc.select_single_node("/config/log/gui/showcount");
	m_nLogGuiShowCount = boost::lexical_cast<int>(node.node().child_value());
	node = doc.select_single_node("/config/log/gui/threadpool");
	m_nLogShowThreadPool = boost::lexical_cast<int>(node.node().child_value());

	node = doc.select_single_node("/config/log/mq/enable");
	m_nLogMqEnable = boost::lexical_cast<int>(node.node().child_value());
	node = doc.select_single_node("/config/log/mq/threadpool");
	m_nLogMqThreadPool = boost::lexical_cast<int>(node.node().child_value());
	node = doc.select_single_node("/config/log/mq/min");
	m_nLogMqMin = boost::lexical_cast<int>(node.node().child_value());
	node = doc.select_single_node("/config/log/mq/max");
	m_nLogMqMax = boost::lexical_cast<int>(node.node().child_value());
	node = doc.select_single_node("/config/log/mq/increase");
	m_nLogMqIncrease = boost::lexical_cast<int>(node.node().child_value());

	pugi::xpath_node_set nodesMQServers = doc.select_nodes("/config/log/mq/mqserver");
	pugi::xpath_node_set::const_iterator itMQServers;

	for (itMQServers = nodesMQServers.begin(); itMQServers != nodesMQServers.end(); ++itMQServers)
	{
		pugi::xpath_node node = *itMQServers;



		std::string mqserver = node.node().child_value();
		
		m_vLogMqServer.push_back(mqserver);
		
	}


	

	return true;
}

/*
int ConfigManager::ConvertIntToBusiType(int val)
{
	switch (val)
	{
	case 1:
		return BUSI_TYPE_STOCK;
	case 2:
		return BUSI_TYPE_CREDIT;
	case 3:
		return BUSI_TYPE_REGISTER;
	case 4:
		return BUSI_TYPE_ACCOUNT;
	case 5:
		return BUSI_TYPE_AUTH;
	case 6:
		return BUSI_TYPE_OPTION;
	case 7:
		return BUSI_TYPE_VERIFY;
	case 0:
	default:
		return BUSI_TYPE_ALL;

	}
}

int ConfigManager::ConvertIntToCounterType(int val)
{
	switch (val)
	{
	case 1:
		return COUNTER_TYPE_HS_T2;
	case 2:
		return COUNTER_TYPE_HS_COM;
	case 3:
		return COUNTER_TYPE_JZ_WIN;
	case 4:
		return COUNTER_TYPE_JZ_LINUX;
	case 5:
		return COUNTER_TYPE_DINGDIAN;
	case 6:
		return COUNTER_TYPE_JSD;
	case 7:
		return COUNTER_TYPE_XINYI;
	default:
		return COUNTER_TYPE_UNKNOWN;
	}
}
*/