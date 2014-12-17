#ifndef CONNECT_MANAGER_H
#define CONNECT_MANAGER_H

#include <map>
#include <string>
#include <mutex>

#include "common.h"
#include "BusinessSystem.h"
#include "BusinessType.h"
#include "Counter.h"
#include "IBusiness.h"



class CConnectManager
{
public:
	
	CConnectManager(void);
	~CConnectManager(void);

	
	std::map<std::string/*系统编号*/, BusinessSystem> systems;

	// 加mutex控制
	IBusiness* GetConnect(std::string sysNo, int busiType, std::string sBranchId);
	void PushConnect(IBusiness * pConn, std::string sysNo, int busiType, std::string sBranchId);
	void CloseConnPool();

	bool GetCounterTypeAndAsyncMode(std::string& systemNo, std::string& businessType, int& counterType, int& counterMode);

	/*
	Counter* GetNextCounter(std::string system, int business, std::string branch);
	int GetCounterCount(std::string system, int business, std::string branch);
	
	*/
};

extern CConnectManager g_ConnectManager;

#endif
