#include "ConnectPool.h"
#include "ConfigManager.h"
#include "FileLog.h"
#include "ConnectT2.h"


ConnectPool gConnectPool;

ConnectPool::ConnectPool()
{
	m_nID = 0;

	

	m_bCreatePool = false;
}

ConnectPool::~ConnectPool(void)
{
	CloseConnPool();
}

void ConnectPool::SetCounterServer(std::vector<Counter> vCounter)
{
	m_vCounter = vCounter;
}

bool ConnectPool::CreateConnPool()
{
	gFileLog::instance().Log("开始创建连接池");


	// 初始化连接数=柜台服务器数 * m_nConnPoolMin
	// 假设服务器有4个, m_nConnPoolMin=2, 创建后s1, s2, s3, s4, s1,s2,s3,s4

	int totalConnCount = m_vCounter.size() * gConfigManager::instance().m_nConnectPoolMax;

	for (int i=0; i < gConfigManager::instance().m_nConnectPoolMax; i++)
	{
		for (std::vector<Counter>::iterator pos = m_vCounter.begin(); pos != m_vCounter.end(); pos++)
		{
			
			IConnect * pConn = new ConnectT2(m_nID, *pos);
			if (pConn->CreateConnect())
			{
				m_pool.push(pConn);
				m_nID++;
			}
			else
			{
				delete pConn;
			}

		} // end for 柜台服务器数
	} // end for 初始次数
	

	

	if (m_pool.queue_.size() != totalConnCount)
	{
		std::string msg = "建立连接池失败";
		gFileLog::instance().Log(msg);
		

		m_bCreatePool = false;
	}
	else
	{
		std::string msg = "建立连接池成功";
		gFileLog::instance().Log(msg);


		m_bCreatePool = true;
	}


	return m_bCreatePool;
}

/*
bool ConnectPool::IncreaseConnPool()
{
	int nOldSize = m_pool.queue_.size();



	// 增长数=柜台服务器数 * m_nConnPoolIncrease
	for (int i=0; i < gConfigManager::instance().m_nConnectPoolIncrease; i++)
	{
		for (std::vector<Counter>::iterator pos = m_vCounter.begin(); pos != m_vCounter.end(); pos++)
		{

			Connect * pConn = new Connect(m_nID, *pos);
			if (pConn->CreateConnect())
			{
				m_pool.push(pConn);
				m_nID++;
			}
			else
			{
				delete pConn;
			}

		} // end for 柜台服务器数
	} // end for 增长次数
		

	int m_nConnCount = m_pool.queue_.size();
	

	if (m_nConnCount == nOldSize)
	{
			
		std::string msg = "扩充连接池失败";
		gFileLog::instance().Log(msg);

		m_bCreatePool = false;
	}
	else
	{
		std::string msg = "扩充连接池成功";
		gFileLog::instance().Log(msg);


		m_bCreatePool = true;
	}

	return m_bCreatePool;
}
*/


// 获取连接的遍历采用循环方法，不采用随机方法
 IConnect* ConnectPool::GetConnect()
{
	std::string msg;
	IConnect * pConn = NULL;

	/*
	if (m_pool.queue_.empty())
	{
		if (!IncreaseConnPool())
			return NULL;
	}
	*/

	pConn = m_pool.pop();
	if (pConn == NULL)
	{
		msg = "获取连接，失败";
		gFileLog::instance().Log(msg);

		return NULL;
	}

	/*
	if (pConn->IsTimeout())
	{
		if (!pConn->ReConnect())
		{
			msg = "获取连接超时重连失败" + pConn->GetConnectInfo();
			gFileLog::instance().Log(msg);

			return NULL;
		}
	}
	*/

	msg = "获取连接成功, " + pConn->GetConnectInfo();
	gFileLog::instance().Log(msg);

	return pConn;
}

void ConnectPool::PushConnect(IConnect* pConn)
{
	if (pConn == NULL)
		return;

	std::string msg = "释放连接, " + pConn->GetConnectInfo();
	gFileLog::instance().Log(msg);

	msg = "释放连接: 归还前大小" + boost::lexical_cast<std::string>(m_pool.queue_.size());
	gFileLog::instance().Log(msg);

	m_pool.push(pConn);

	msg = "释放连接: 归还后大小" + boost::lexical_cast<std::string>(m_pool.queue_.size());
	gFileLog::instance().Log(msg);
}

void ConnectPool::CloseConnPool()
{
	if (m_bCreatePool)
	{
		std::string msg = "关闭连接池";
		gFileLog::instance().Log(msg);

		m_bCreatePool = false;

		m_pool.stop();

	
		for (std::deque<IConnect*>::iterator pos = m_pool.queue_.begin(); pos != m_pool.queue_.end(); pos++)
		{
			IConnect * pConn = *pos;

			if (pConn != NULL)
			{
				pConn->CloseConnect();
				delete pConn;
				pConn = NULL;
			}
		}
	}
	
}

bool ConnectPool::IsCreatePoolSuccess()
{
	return m_bCreatePool;
}
