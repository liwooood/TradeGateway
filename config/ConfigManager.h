#ifndef _CONFIG_MANAGER_H_
#define _CONFIG_MANAGER_H_

#include <map>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/thread/detail/singleton.hpp>


#include "common.h"
#include "./connectpool/counter.h"
#include "./connectpool/businesssystem.h"


class ConfigManager
{
public:
	ConfigManager(void);
	~ConfigManager(void);

	// 配置文件路径
	std::string m_sPath;
	std::string m_sExeFile;
	bool LoadConfig(std::string sPath="");



	// 通信层
	std::string m_sIp;

	

	// 准备废弃
	int m_nSslPort;
	int m_nSslWorkerThreadPool;
	int m_nSslSendThreadPool;
	int m_nSslEnable;
	int m_nAuth;
	
	// 准备废弃	
	int m_nTcpPort;
	int m_nTcpWorkerThreadPool;
	int m_nTcpSendThreadPool;
	int m_nTcpEnable;


	// 
	int m_nSslNewPort;
	int m_nSslNewWorkerThreadPool;
	int m_nSslNewSendThreadPool;
	int m_nSslNewEnable;
	int m_nNewAuth;
	

	//
	int m_nTcpNewPort;
	int m_nTcpNewWorkerThreadPool;
	int m_nTcpNewSendThreadPool;
	int m_nTcpNewEnable;
	
	// 压缩大小
	int zlib;

	// 运行模式
	int nRunMode;

	// 柜台
	int m_nConnectPoolEnable;
	int m_nConnectPoolConnectTimeout;
	int m_nConnectPoolReadWriteTimeout;
	std::vector<Counter> m_ConnectPoolCounters;
	int m_nConnectPoolMin;
	int m_nConnectPoolMax;
	int m_nConnectPoolIncrease;
	int m_nConnectRetry;
	int m_nBusinessRetry; // 业务失败重试



	//int m_nStockEnable;
	//std::map<std::string, std::vector<Counter>> m_Stock;

	//int m_nCreditEnable;
	//std::map<std::string, std::vector<Counter>> m_Credit;

	//std::map<std::string, std::vector<Counter>> m_Account;

	//std::map<std::string, std::vector<Counter>> m_Option;

	//std::map<std::string, std::vector<Counter>> m_Auth;

	
	// 日志
	
	// 日志级别
	int m_nLogLevel;
	int m_nResponseLen;

	int m_nFilterFuncId;

	// 过滤输出字段
	int m_nFilterField;

	// 客户号
	std::map<std::string, int> m_mCustIds;
	std::string m_sFilterCustId; // custid=123&custid=456

	// 文件系统日志
	int m_nLogFileEnable;
	std::string m_sLogFilePath;
	std::string m_sLogDirSystem; // ?
	int m_nLogFileThreadPool;




	// 界面日志
	int m_nLogGuiEnable;
	int m_nLogGuiShowCount;
	int m_nShowDebugLog;
	int m_nShowInfoLog;
	int m_nShowWarnLog;
	int m_nShowErrorLog;
	int m_nLogShowThreadPool;

	// 分布式日志系统
	int m_nLogMqEnable;
	int m_nLogMqThreadPool;
	int m_nLogMqMin;
	int m_nLogMqMax;
	int m_nLogMqIncrease;
	std::vector<std::string> m_vLogMqServer;


	
private:
	bool ReadSystemFromXML(std::string systemFile);

};
typedef boost::detail::thread::singleton<ConfigManager> gConfigManager;

#endif


