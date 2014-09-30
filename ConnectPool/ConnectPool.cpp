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
	gFileLog::instance().Log("��ʼ�������ӳ�");


	// ��ʼ��������=��̨�������� * m_nConnPoolMin
	// �����������4��, m_nConnPoolMin=2, ������s1, s2, s3, s4, s1,s2,s3,s4

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

		} // end for ��̨��������
	} // end for ��ʼ����
	

	

	if (m_pool.queue_.size() != totalConnCount)
	{
		std::string msg = "�������ӳ�ʧ��";
		gFileLog::instance().Log(msg);
		

		m_bCreatePool = false;
	}
	else
	{
		std::string msg = "�������ӳسɹ�";
		gFileLog::instance().Log(msg);


		m_bCreatePool = true;
	}


	return m_bCreatePool;
}

/*
bool ConnectPool::IncreaseConnPool()
{
	int nOldSize = m_pool.queue_.size();



	// ������=��̨�������� * m_nConnPoolIncrease
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

		} // end for ��̨��������
	} // end for ��������
		

	int m_nConnCount = m_pool.queue_.size();
	

	if (m_nConnCount == nOldSize)
	{
			
		std::string msg = "�������ӳ�ʧ��";
		gFileLog::instance().Log(msg);

		m_bCreatePool = false;
	}
	else
	{
		std::string msg = "�������ӳسɹ�";
		gFileLog::instance().Log(msg);


		m_bCreatePool = true;
	}

	return m_bCreatePool;
}
*/


// ��ȡ���ӵı�������ѭ���������������������
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
		msg = "��ȡ���ӣ�ʧ��";
		gFileLog::instance().Log(msg);

		return NULL;
	}

	/*
	if (pConn->IsTimeout())
	{
		if (!pConn->ReConnect())
		{
			msg = "��ȡ���ӳ�ʱ����ʧ��" + pConn->GetConnectInfo();
			gFileLog::instance().Log(msg);

			return NULL;
		}
	}
	*/

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

	msg = "�ͷ�����: �黹ǰ��С" + boost::lexical_cast<std::string>(m_pool.queue_.size());
	gFileLog::instance().Log(msg);

	m_pool.push(pConn);

	msg = "�ͷ�����: �黹���С" + boost::lexical_cast<std::string>(m_pool.queue_.size());
	gFileLog::instance().Log(msg);
}

void ConnectPool::CloseConnPool()
{
	if (m_bCreatePool)
	{
		std::string msg = "�ر����ӳ�";
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
