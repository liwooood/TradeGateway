#ifndef _CONNECT_MANAGER_H_
#define _CONNECT_MANAGER_H_

#include <map>
#include <string>

#include <mutex>

#include "common.h"
#include "BusinessSystem.h"
#include "BusinessType.h"
#include "Counter.h"



class CConnectManager
{
public:
	std::map<std::string/*系统编号*/, BusinessSystem> systems;


public:
	CConnectManager(void);
	~CConnectManager(void);

	Counter* GetNextCounter(std::string system, int business, std::string branch);
	int GetCounterCount(std::string system, int business, std::string branch);
	int GetCounterType(std::string SystemNo, std::string busiType);


	//保证线程安全?
	//Connect* GetConnect(std::string sysNo, int busiType, std::string sBranchId);
	//void PushConnect(Connect * pConn, std::string sysNo, int busiType, std::string sBranchId);
	//void CloseConnPool();

	
	

	// configmanager不需要定义std::map<std::string, BusinessSystem> systems;
	

	//            系统编号         业务类型        营业部列表   连接池
	//std::map<std::string, std::map<enum, std::map<std::string, ConnectPool>> >
	// connectpool --> queue --> connect

	
};

extern CConnectManager g_ConnectManager;

#endif
