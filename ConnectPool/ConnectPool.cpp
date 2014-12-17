#include "stdafx.h"

#include <boost/lexical_cast.hpp>

#include "ConnectPool.h"
#include "ConfigManager.h"
#include "FileLog.h"

#include "IBusiness.h"
#include "TradeBusinessHS.h"
#include "TradeBusinessJZ.h"
#include "TradeBusinessDD.h"
#include "TradeBusinessJSD.h"
#include "TradeBusinessTest.h"




ConnectPool::ConnectPool()
{
	m_nID = 0;
	m_bCreatePool = false;
}

ConnectPool::~ConnectPool(void)
{
	
}

void ConnectPool::SetCounterServer(std::vector<Counter> vCounter)
{
	m_vCounter = vCounter;
}

bool ConnectPool::CreateConnPool()
{
	
	int count = 0;

	// 初始化连接数=柜台服务器数 * m_nConnPoolMin
	// 假设服务器有4个, m_nConnPoolMin=2, 创建后s1, s2, s3, s4, s1,s2,s3,s4

	int totalConnCount = m_vCounter.size() * gConfigManager::instance().m_nConnectPoolMin;

	for (int i=0; i < gConfigManager::instance().m_nConnectPoolMin; i++)
	{
		for (std::vector<Counter>::iterator pos = m_vCounter.begin(); pos != m_vCounter.end(); pos++)
		{
			Counter counter = *pos;

			IBusiness * pConn;

			if (counter.m_nCounterType == COUNTER_TYPE_HS_T2)
			{
				pConn = new TradeBusinessHS(m_nID, counter);
			}
			if (counter.m_nCounterType == COUNTER_TYPE_JZ_WIN)
			{
				pConn = new TradeBusinessJZ(m_nID, counter);
			}
			if (counter.m_nCounterType == COUNTER_TYPE_DINGDIAN)
			{
				pConn = new TradeBusinessDD(m_nID, counter);
			}
			if (counter.m_nCounterType == COUNTER_TYPE_JSD)
			{
				pConn = new TradeBusinessJSD(m_nID, counter);
			}
			if (counter.m_nCounterType == COUNTER_TYPE_XINYI || counter.m_nCounterType == COUNTER_TYPE_TEST)
			{
				pConn = new TradeBusinessTest(m_nID, counter);
			}

			
			if (pConn->CreateConnect())
			{
				m_pool.push(pConn);
				m_nID++;

				count++;
			}
			else
			{
				delete pConn;
			}

		} // end for 柜台服务器数
	} // end for 初始次数
	

	

	if (count != totalConnCount)
	{
		

		m_bCreatePool = false;
	}
	else
	{
		

		m_bCreatePool = true;
	}


	return m_bCreatePool;
}


bool ConnectPool::IncreaseConnPool()
{
	int count = 0;

	int totalConnCount = m_vCounter.size() * gConfigManager::instance().m_nConnectPoolIncrease;



	// 增长数=柜台服务器数 * m_nConnPoolIncrease
	for (int i=0; i < gConfigManager::instance().m_nConnectPoolIncrease; i++)
	{
		for (std::vector<Counter>::iterator pos = m_vCounter.begin(); pos != m_vCounter.end(); pos++)
		{
			Counter counter = *pos;

			IBusiness * pConn;

			if (counter.m_nCounterType == COUNTER_TYPE_HS_T2)
			{
				pConn = new TradeBusinessHS(m_nID, counter);
			}
			if (counter.m_nCounterType == COUNTER_TYPE_JZ_WIN)
			{
				pConn = new TradeBusinessJZ(m_nID, counter);
			}
			if (counter.m_nCounterType == COUNTER_TYPE_DINGDIAN)
			{
				pConn = new TradeBusinessDD(m_nID, counter);
			}
			if (counter.m_nCounterType == COUNTER_TYPE_JSD)
			{
				pConn = new TradeBusinessJSD(m_nID, counter);
			}
			if (counter.m_nCounterType == COUNTER_TYPE_XINYI || counter.m_nCounterType == COUNTER_TYPE_TEST)
			{
				pConn = new TradeBusinessTest(m_nID, counter);
			}

			if (pConn->CreateConnect())
			{
				m_pool.push(pConn);
				m_nID++;

				count++;
			}
			else
			{
				delete pConn;
			}

		} // end for 柜台服务器数
	} // end for 增长次数
		


	

	if (count == totalConnCount)
	{
			
		

		m_bCreatePool = false;
	}
	else
	{
		

		m_bCreatePool = true;
	}

	return m_bCreatePool;
}



// 获取连接的遍历采用循环方法，不采用随机方法
 IBusiness* ConnectPool::GetConnect()
{
	std::string msg;
	IBusiness * pConn = NULL;

	
	if (m_pool.queue.empty())
	{
		if (!IncreaseConnPool())
			return NULL;
	}
	

	pConn = m_pool.pop();
	if (pConn == NULL)
	{
		msg = "获取连接，失败";
		gFileLog::instance().Log(msg);

		return NULL;
	}


	msg = "获取连接成功, " + pConn->GetConnectInfo();
	gFileLog::instance().Log(msg);

	return pConn;
}

void ConnectPool::PushConnect(IBusiness* pConn)
{
	if (pConn == NULL)
		return;

	std::string msg = "释放连接, " + pConn->GetConnectInfo();
	gFileLog::instance().Log(msg);

	

	m_pool.push(pConn);

}

void ConnectPool::CloseConnPool()
{
	
		m_pool.stop();

	
		for (std::deque<IBusiness*>::iterator pos = m_pool.queue.begin(); pos != m_pool.queue.end(); pos++)
		{
			IBusiness * pConn = *pos;

			if (pConn != NULL)
			{
				pConn->CloseConnect();
				delete pConn;
				pConn = NULL;
			}
		}

		m_bCreatePool = false;
	
	
}

bool ConnectPool::IsCreatePoolSuccess()
{
	return m_bCreatePool;
}
