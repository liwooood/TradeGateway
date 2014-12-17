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


	// ֱ������std::deque�ȽϺ�
	typedef QueueThreadSafe<IBusiness*> conn_queue_type;
	conn_queue_type m_pool;

	//��ʹ�ö��У��´θ�
	//std::vector<conn_queue_type> pool;

	
	int m_nID; // �������к�
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
