#ifndef _CONNECT_POOL_H_
#define _CONNECT_POOL_H_

#include <vector>

#include "ThreadSafeQueue/job_queue.h"
#include "Connect.h"

#include "common.h"
#include "Counter.h"


class ConnectPool
{
private:
	// ֱ������std::deque�ȽϺ�
	typedef job_queue<CConnect*> conn_queue_type;
	conn_queue_type m_pool;
	//std::vector<conn_queue_type> pool;

	std::vector<Counter> m_vCounter;
	int m_nID; // �������к�
	bool m_bCreatePool;

public:
	ConnectPool(std::vector<Counter> vCounter);
	~ConnectPool(void);

	bool CreateConnPool();
	bool IncreaseConnPool();
	void CloseConnPool();

	CConnect* GetConnect();
	void PushConnect(CConnect * pConn);


	
	bool IsCreatePoolSuccess();

};
#endif
