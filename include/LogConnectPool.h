#ifndef _MQ_MANAGER_H_
#define _MQ_MANAGER_H_

//#include <boost/asio.hpp>
//#include <boost/bind.hpp>
//#include <boost/lexical_cast.hpp>
//#include <boost/noncopyable.hpp>
//#include <boost/thread.hpp>
#include <boost/thread/detail/singleton.hpp>


#include "QueueThreadSafe.h"

class LogConnect;

class LogConnectPool
{
public:
	LogConnectPool(void);
	~LogConnectPool(void);

	BOOL CreateConnectPool();
	BOOL IncreaseConnPool();
	void CloseConnectPool();

	LogConnect* GetConnect();
	void PushConnect(LogConnect * pConn);

	
	

private:
	QueueThreadSafe<LogConnect*> m_pool;
	int m_nID; // �������к�
	int m_nConnCount; // ������
	

	int m_nMin;
	int m_nIncrease;
	int m_nMax;

	std::string logFileName;
};

typedef boost::detail::thread::singleton<LogConnectPool> gLogConnectPool;

#endif
