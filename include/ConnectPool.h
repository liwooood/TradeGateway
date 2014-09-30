#ifndef _CONNECT_POOL_H_
#define _CONNECT_POOL_H_

#include <vector>

#include "job_queue.h"
#include "IConnect.h"

#include "common.h"
#include "Counter.h"


class ConnectPool
{
private:
	// ֱ������std::deque�ȽϺ�
	typedef job_queue<IConnect*> conn_queue_type;
	conn_queue_type m_pool;
	//std::vector<conn_queue_type> pool;

	std::vector<Counter> m_vCounter;
	int m_nID; // �������к�
	bool m_bCreatePool;

public:
	ConnectPool();
	~ConnectPool(void);

	void SetCounterServer(std::vector<Counter> vCounter);

	bool CreateConnPool();
	bool IncreaseConnPool();
	void CloseConnPool();

	IConnect* GetConnect();
	void PushConnect(IConnect * pConn);


	
	bool IsCreatePoolSuccess();

};

extern ConnectPool gConnectPool;

#endif
