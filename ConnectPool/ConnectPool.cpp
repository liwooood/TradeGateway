#include "stdafx.h"

#include <boost/lexical_cast.hpp>

#include "ConnectPool.h"
#include "ConfigManager.h"
#include "FileLog.h"

#include "IConnect.h"
#include "ConnectHS.h"
#include "ConnectJZ.h"
#include "ConnectDD.h"
#include "ConnectJSD.h"
#include "ConnectTest.h"




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

	// ��ʼ��������=��̨�������� * m_nConnPoolMin
	// �����������4��, m_nConnPoolMin=2, ������s1, s2, s3, s4, s1,s2,s3,s4

	int totalConnCount = m_vCounter.size() * gConfigManager::instance().m_nConnectPoolMin;

	for (int i=0; i < gConfigManager::instance().m_nConnectPoolMin; i++)
	{
		for (std::vector<Counter>::iterator pos = m_vCounter.begin(); pos != m_vCounter.end(); pos++)
		{
			Counter counter = *pos;

			IConnect * pConn;

			if (counter.m_nCounterType == COUNTER_TYPE_HS_T2)
			{
				pConn = new ConnectHS(m_nID, counter);
			}
			if (counter.m_nCounterType == COUNTER_TYPE_JZ_WIN)
			{
				pConn = new ConnectJZ(m_nID, counter);
			}
			if (counter.m_nCounterType == COUNTER_TYPE_DINGDIAN)
			{
				pConn = new ConnectDD(m_nID, counter);
			}
			if (counter.m_nCounterType == COUNTER_TYPE_JSD)
			{
				pConn = new ConnectJSD(m_nID, counter);
			}
			if (counter.m_nCounterType == COUNTER_TYPE_XINYI || counter.m_nCounterType == COUNTER_TYPE_TEST)
			{
				pConn = new ConnectTest(m_nID, counter);
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

		} // end for ��̨��������
	} // end for ��ʼ����
	

	

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



	// ������=��̨�������� * m_nConnPoolIncrease
	for (int i=0; i < gConfigManager::instance().m_nConnectPoolIncrease; i++)
	{
		for (std::vector<Counter>::iterator pos = m_vCounter.begin(); pos != m_vCounter.end(); pos++)
		{
			Counter counter = *pos;

			IConnect * pConn;

			if (counter.m_nCounterType == COUNTER_TYPE_HS_T2)
			{
				pConn = new ConnectHS(m_nID, counter);
			}
			if (counter.m_nCounterType == COUNTER_TYPE_JZ_WIN)
			{
				pConn = new ConnectJZ(m_nID, counter);
			}
			if (counter.m_nCounterType == COUNTER_TYPE_DINGDIAN)
			{
				pConn = new ConnectDD(m_nID, counter);
			}
			if (counter.m_nCounterType == COUNTER_TYPE_JSD)
			{
				pConn = new ConnectJSD(m_nID, counter);
			}
			if (counter.m_nCounterType == COUNTER_TYPE_XINYI || counter.m_nCounterType == COUNTER_TYPE_TEST)
			{
				pConn = new ConnectTest(m_nID, counter);
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

		} // end for ��̨��������
	} // end for ��������
		


	

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



// ��ȡ���ӵı�������ѭ���������������������
 IConnect* ConnectPool::GetConnect()
{
	std::string msg;
	IConnect * pConn = NULL;

	
	if (m_pool.queue.empty())
	{
		if (!IncreaseConnPool())
			return NULL;
	}
	

	pConn = m_pool.pop();
	if (pConn == NULL)
	{
		msg = "��ȡ���ӣ�ʧ��";
		gFileLog::instance().Log(msg);

		return NULL;
	}


	msg = "��ȡ���ӳɹ�, " + pConn->GetConnectInfo();
	gFileLog::instance().Log(msg);

	return pConn;
}

void ConnectPool::PushConnect(IConnect* pConn)
{
	if (pConn == NULL)
		return;

	std::string msg = "�ͷ�����, " + pConn->GetConnectInfo();
	gFileLog::instance().Log(msg);

	

	m_pool.push(pConn);

}

void ConnectPool::CloseConnPool()
{
	
		m_pool.stop();

	
		for (std::deque<IConnect*>::iterator pos = m_pool.queue.begin(); pos != m_pool.queue.end(); pos++)
		{
			IConnect * pConn = *pos;

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
