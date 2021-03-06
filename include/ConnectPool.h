#ifndef CONNECT_POOL_H
#define CONNECT_POOL_H

#include <vector>

#include "QueueThreadSafe.h"
#include "IBusiness.h"

#include "common.h"
#include "Counter.h"


class ConnectPool
{
private:
	


	// 直接整合std::deque比较好
	typedef QueueThreadSafe<IBusiness*> conn_queue_type;
	conn_queue_type m_pool;

	//不使用队列，下次改
	//std::vector<conn_queue_type> pool;

	
	int m_nID; // 连接序列号
	bool m_bCreatePool;
	std::string logFile;
	std::string systemNo;
	std::string busiType;
	int counterType;
	std::vector<Counter> m_vCounter;

public:
	ConnectPool(std::string systemNo, std::string busiType, int counterType, std::vector<Counter> vCounter);
	~ConnectPool(void);



	bool CreateConnPool();
	bool IncreaseConnPool();
	void CloseConnPool();

	IBusiness* GetConnect();
	void PushConnect(IBusiness * pConn);


	
	bool IsCreatePoolSuccess();

};



#endif
