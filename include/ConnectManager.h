#ifndef CONNECT_MANAGER_H
#define CONNECT_MANAGER_H

#include <map>
#include <string>
#include <mutex>

#include "common.h"
#include "BusinessSystem.h"
#include "BusinessType.h"
#include "Counter.h"
#include "IConnect.h"



class CConnectManager
{
public:
	
	CConnectManager(void);
	~CConnectManager(void);

	
	std::map<std::string/*系统编号*/, BusinessSystem> systems;

	// 加mutex控制
	IConnect* GetConnect(std::string sysNo, int busiType, std::string sBranchId);
	void PushConnect(IConnect * pConn, std::string sysNo, int busiType, std::string sBranchId);
	void CloseConnPool();

	/*
	Counter* GetNextCounter(std::string system, int business, std::string branch);
	int GetCounterCount(std::string system, int business, std::string branch);
	bool GetCounterTypeAndAsyncMode(std::string& SystemNo, std::string& busiType, int& counterType, int& asyncMode);
	*/
};

extern CConnectManager g_ConnectManager;

#endif
