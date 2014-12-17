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
	std::vector<Counter> m_vCounter;


	// 直接整合std::deque比较好
	typedef QueueThreadSafe<IBusiness*> conn_queue_type;
	conn_queue_type m_pool;

	//不使用队列，下次改
	//std::vector<conn_queue_type> pool;

	
	int m_nID; // 连接序列号
	bool m_bCreatePool;

public:
	ConnectPool();
	~ConnectPool(void);

	void SetCounterServer(std::vector<Counter> vCounter);

	bool CreateConnPool();
	bool IncreaseConnPool();
	void CloseConnPool();

	IBusiness* GetConnect();
	void PushConnect(IBusiness * pConn);


	
	bool IsCreatePoolSuccess();

};



#endif
