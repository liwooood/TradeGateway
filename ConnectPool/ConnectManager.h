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
	std::map<std::string/*ϵͳ���*/, BusinessSystem> systems;


public:
	CConnectManager(void);
	~CConnectManager(void);

	Counter* GetNextCounter(std::string system, int business, std::string branch);
	int GetCounterCount(std::string system, int business, std::string branch);
	int GetCounterType(std::string SystemNo, std::string busiType);


	//��֤�̰߳�ȫ?
	//Connect* GetConnect(std::string sysNo, int busiType, std::string sBranchId);
	//void PushConnect(Connect * pConn, std::string sysNo, int busiType, std::string sBranchId);
	//void CloseConnPool();

	
	

	// configmanager����Ҫ����std::map<std::string, BusinessSystem> systems;
	

	//            ϵͳ���         ҵ������        Ӫҵ���б�   ���ӳ�
	//std::map<std::string, std::map<enum, std::map<std::string, ConnectPool>> >
	// connectpool --> queue --> connect

	
};

extern CConnectManager g_ConnectManager;

#endif
