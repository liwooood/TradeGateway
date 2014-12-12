#include "stdafx.h"

#include "LogConnectPool.h"
#include "LogConnect.h"

#include "ConfigManager.h"
#include "FileLog.h"


LogConnectPool::LogConnectPool(void)
{
	m_nID = 0;
	m_nConnCount = 0;
	logFileName = "�ֲ�ʽ��־";
}


LogConnectPool::~LogConnectPool(void)
{
}

BOOL LogConnectPool::CreateConnectPool()
{
	std::string msg = "��ʼ������־���ӳ�";
	gFileLog::instance().Log(msg, 4,logFileName);

	bool bRet = false;

	
	//m_bCreatePool = true;

	// ��ʼ��������=��̨�������� * m_nConnPoolMin
	// �����������4��, m_nConnPoolMin=2, ������s1, s2, s3, s4, s1,s2,s3,s4

	for (int i=0; i < gConfigManager::instance().m_nLogMqMin; i++)
	{
		for (std::vector<std::string>::iterator pos = gConfigManager::instance().m_vLogMqServer.begin(); pos != gConfigManager::instance().m_vLogMqServer.end(); pos++)
		{
			
			std::string server = *pos;

			LogConnect * pConn = new LogConnect();
			if (pConn->Connect(server))
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
	

	m_nConnCount = m_pool.queue.size();

	if (m_nConnCount == 0)
	{
		std::string msg = "������־���ӳ�ʧ��";
		gFileLog::instance().Log(msg, 4,logFileName);
		

		bRet = false;
	}
	else
	{
		std::string msg = "������־���ӳسɹ�";
		gFileLog::instance().Log(msg, 4,logFileName);


		bRet = true;
	}


	return bRet;
}

BOOL LogConnectPool::IncreaseConnPool()
{
	BOOL bRet = FALSE;

	int nOldSize = m_pool.queue.size();



	// ������=��̨�������� * m_nConnPoolIncrease
	for (int i=0; i < gConfigManager::instance().m_nLogMqIncrease; i++)
	{
		for (std::vector<std::string>::iterator pos = gConfigManager::instance().m_vLogMqServer.begin(); pos != gConfigManager::instance().m_vLogMqServer.end(); pos++)
		{
			std::string server = *pos;

			LogConnect * pConn = new LogConnect();
			if (pConn->Connect(server))
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
		

	m_nConnCount = m_pool.queue.size();

	if (m_nConnCount == nOldSize)
	{
			
		std::string msg = "������־���ӳ�ʧ��";
		gFileLog::instance().Log(msg, 4,logFileName);

		bRet = FALSE;
	}
	else
	{
		std::string msg = "������־���ӳسɹ�";
		gFileLog::instance().Log(msg, 4,logFileName);


		bRet = TRUE;
	}

	return bRet;
}

void LogConnectPool::CloseConnectPool()
{
	std::string msg = "�ر����ӳ�";
	gFileLog::instance().Log(msg, 4,logFileName);

	//m_bCreatePool = false;

	m_pool.stop();

	/*
	����һ
	Connect * pConn = m_pool.pop();
	*/

	// ������
	for (std::deque<LogConnect*>::iterator pos = m_pool.queue.begin(); pos != m_pool.queue.end(); pos++)
	{
		LogConnect * pConn = *pos;

		if (pConn != NULL)
		{
			pConn->Close();
			delete pConn;
			pConn = NULL;
		}
	}
}




LogConnect* LogConnectPool::GetConnect()
{
	std::string msg;

	if (m_pool.queue.empty())
	{
		if (!IncreaseConnPool())
			return NULL;

	}

	LogConnect *pConn = m_pool.pop();
	if (pConn == NULL)
	{
		msg = "��ȡ���ӣ�ʧ��";
		gFileLog::instance().Log(msg, 4,logFileName);

		return NULL;
	}



	msg = "��ȡ���ӳɹ�";
	gFileLog::instance().Log(msg, 4, logFileName);

	return pConn;
}

void LogConnectPool::PushConnect(LogConnect * pConn)
{
	if (pConn == NULL)
		return;

	std::string msg = "�ͷ�����, " ;//+ pConn->GetConnectInfo();
	gFileLog::instance().Log(msg, 4, logFileName);

	msg = "�ͷ�����: �黹ǰ��С" + boost::lexical_cast<std::string>(m_pool.queue.size());
	gFileLog::instance().Log(msg, 4, logFileName);

	m_pool.push(pConn);

	msg = "�ͷ�����: �黹���С" + boost::lexical_cast<std::string>(m_pool.queue.size());
	gFileLog::instance().Log(msg, 4, logFileName);
}
