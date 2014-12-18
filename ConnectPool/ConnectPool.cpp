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




ConnectPool::ConnectPool(std::string systemNo, std::string busiType, int counterType, std::vector<Counter> vCounter)
{
	this->systemNo = systemNo;
	this->busiType = busiType; 
	this->counterType = counterType;
	m_vCounter = vCounter;

	m_nID = 0;
	m_bCreatePool = false;

	//<!-- 柜台类型：1.恒生T2, 2.恒生COM 3.金证windows 4.金证linux， 5.顶点  6.金仕达， 7.核新同花顺统一认证 8.仿真柜台-->
	if (counterType == 1)
	{
		logFile = "柜台\\恒生";
	}
	if (counterType == 3)
	{
		logFile = "柜台\\金证";
	}
	if (counterType == 5)
	{
		logFile = "柜台\\顶点";
	}
	if (counterType == 6)
	{
		logFile = "柜台\\金仕达";
	}
	if (counterType == 8)
	{
		logFile = "柜台\\仿真";
	}

}

ConnectPool::~ConnectPool(void)
{
	
}


bool ConnectPool::CreateConnPool()
{
	gFileLog::instance().debug(logFile, "开始创建连接池");

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
		gFileLog::instance().error(logFile, "创建连接池失败");

		m_bCreatePool = false;
	}
	else
	{
		gFileLog::instance().debug(logFile, "创建连接池成功");

		m_bCreatePool = true;
	}


	return m_bCreatePool;
}


bool ConnectPool::IncreaseConnPool()
{
	gFileLog::instance().debug(logFile, "开始扩充连接池");
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
			
		gFileLog::instance().error(logFile, "扩充连接池失败");

		m_bCreatePool = false;
	}
	else
	{
		gFileLog::instance().debug(logFile, "扩充连接池成功");

		m_bCreatePool = true;
	}

	return m_bCreatePool;
}



// 获取连接的遍历采用循环方法，不采用随机方法
 IBusiness* ConnectPool::GetConnect()
{
	
	IBusiness * pConn = NULL;

	
	if (m_pool.queue.empty())
	{
		if (!IncreaseConnPool())
		{
			gFileLog::instance().error(logFile, "连接池空，扩充失败，获取连接失败");
			return NULL;
		}
	}
	

	pConn = m_pool.pop();
	if (pConn == NULL)
	{
	
		gFileLog::instance().error(logFile, "获取连接==null");

		return NULL;
	}



	gFileLog::instance().debug(logFile, "获取连接成功, " + pConn->GetConnectInfo());

	return pConn;
}

void ConnectPool::PushConnect(IBusiness* pConn)
{
	if (pConn == NULL)
		return;


	gFileLog::instance().debug(logFile, "释放连接, " + pConn->GetConnectInfo());

	

	m_pool.push(pConn);

}

void ConnectPool::CloseConnPool()
{
	gFileLog::instance().debug(logFile, "开始关闭连接池");
	
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
	
	gFileLog::instance().debug(logFile, "开始关闭连接池结束");
}

bool ConnectPool::IsCreatePoolSuccess()
{
	return m_bCreatePool;
}
