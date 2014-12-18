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

	//<!-- ��̨���ͣ�1.����T2, 2.����COM 3.��֤windows 4.��֤linux�� 5.����  6.���˴ 7.����ͬ��˳ͳһ��֤ 8.�����̨-->
	if (counterType == 1)
	{
		logFile = "��̨\\����";
	}
	if (counterType == 3)
	{
		logFile = "��̨\\��֤";
	}
	if (counterType == 5)
	{
		logFile = "��̨\\����";
	}
	if (counterType == 6)
	{
		logFile = "��̨\\���˴�";
	}
	if (counterType == 8)
	{
		logFile = "��̨\\����";
	}

}

ConnectPool::~ConnectPool(void)
{
	
}


bool ConnectPool::CreateConnPool()
{
	gFileLog::instance().debug(logFile, "��ʼ�������ӳ�");

	int count = 0;

	// ��ʼ��������=��̨�������� * m_nConnPoolMin
	// �����������4��, m_nConnPoolMin=2, ������s1, s2, s3, s4, s1,s2,s3,s4

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

		} // end for ��̨��������
	} // end for ��ʼ����
	

	

	if (count != totalConnCount)
	{
		gFileLog::instance().error(logFile, "�������ӳ�ʧ��");

		m_bCreatePool = false;
	}
	else
	{
		gFileLog::instance().debug(logFile, "�������ӳسɹ�");

		m_bCreatePool = true;
	}


	return m_bCreatePool;
}


bool ConnectPool::IncreaseConnPool()
{
	gFileLog::instance().debug(logFile, "��ʼ�������ӳ�");
	int count = 0;

	int totalConnCount = m_vCounter.size() * gConfigManager::instance().m_nConnectPoolIncrease;



	// ������=��̨�������� * m_nConnPoolIncrease
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

		} // end for ��̨��������
	} // end for ��������
		


	

	if (count == totalConnCount)
	{
			
		gFileLog::instance().error(logFile, "�������ӳ�ʧ��");

		m_bCreatePool = false;
	}
	else
	{
		gFileLog::instance().debug(logFile, "�������ӳسɹ�");

		m_bCreatePool = true;
	}

	return m_bCreatePool;
}



// ��ȡ���ӵı�������ѭ���������������������
 IBusiness* ConnectPool::GetConnect()
{
	
	IBusiness * pConn = NULL;

	
	if (m_pool.queue.empty())
	{
		if (!IncreaseConnPool())
		{
			gFileLog::instance().error(logFile, "���ӳؿգ�����ʧ�ܣ���ȡ����ʧ��");
			return NULL;
		}
	}
	

	pConn = m_pool.pop();
	if (pConn == NULL)
	{
	
		gFileLog::instance().error(logFile, "��ȡ����==null");

		return NULL;
	}



	gFileLog::instance().debug(logFile, "��ȡ���ӳɹ�, " + pConn->GetConnectInfo());

	return pConn;
}

void ConnectPool::PushConnect(IBusiness* pConn)
{
	if (pConn == NULL)
		return;


	gFileLog::instance().debug(logFile, "�ͷ�����, " + pConn->GetConnectInfo());

	

	m_pool.push(pConn);

}

void ConnectPool::CloseConnPool()
{
	gFileLog::instance().debug(logFile, "��ʼ�ر����ӳ�");
	
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
	
	gFileLog::instance().debug(logFile, "��ʼ�ر����ӳؽ���");
}

bool ConnectPool::IsCreatePoolSuccess()
{
	return m_bCreatePool;
}
