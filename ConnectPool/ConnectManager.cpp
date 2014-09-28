#include "stdafx.h"

#include <boost/lexical_cast.hpp>

#include "ConnectManager.h"
#include "./config/ConfigManager.h"
#include "./output/FileLog.h"
//#include ".\dingdian\fixapi.h"




CConnectManager g_ConnectManager;

CConnectManager::CConnectManager(void)
{
}

CConnectManager::~CConnectManager(void)
{
}





/*
Connect* CConnectManager::GetConnect(std::string sysNo, int busiType, std::string sBranchId)
{
	Connect* pConn = NULL;

	// �Ҳ���ϵͳ��ֱ�ӷ���
	std::map<std::string, BusinessSystem>::iterator itSys;
	itSys = systems.find(sysNo);
	if (itSys == systems.end())
		return NULL;

	// �Ҳ���ҵ�����ͷ���
	BusinessSystem& sys = itSys->second;
	std::map<int, BusinessType >::iterator itBusiType;
	itBusiType = sys.busis.find(busiType);
	if (itBusiType == sys.busis.end())
	{
		return NULL;
	}

	// �Ҳ���Ӫҵ������
	BusinessType& bt = itBusiType->second;
	std::map<std::string, ConnectPool*>::iterator itBranch;
	bool bFoundBranch = false;
	for (itBranch = bt.connPool.begin(); itBranch != bt.connPool.end(); itBranch++)
	{
		std::string branchList = itBranch->first;

		if (branchList == "0000")
		{
			bFoundBranch = true;
			break;
		}

		if (branchList.find(sBranchId) != std::string::npos)
		{
			bFoundBranch = true;
			break;
		}
		else
		{
			bFoundBranch = false;
		}
	}

	if (!bFoundBranch)
	{
		return NULL;
	}

	
	ConnectPool * pool = itBranch->second;
	if (pool->IsCreatePool())
	{
		pConn = pool->GetConnect();

	}
	else
	{
		if (!pool->CreateConnPool())
		{

			return NULL;
		}
		else
		{
			pConn = pool->GetConnect();
		}		
	}

	return pConn;
}

void CConnectManager::PushConnect(Connect * pConn, std::string sysNo, int busiType, std::string sBranchId)
{
	if (pConn == NULL)
		return;

	if (sysNo.empty())
		return ;


	// �Ҳ���ϵͳ��ֱ�ӷ���
	std::map<std::string, BusinessSystem>::iterator itSys;
	itSys = systems.find(sysNo);
	if (itSys == systems.end())
		return;

	// �Ҳ���ҵ�����ͷ���
	BusinessSystem& sys = itSys->second;
	std::map<int, BusinessType >::iterator itBusiType;
	itBusiType = sys.busis.find(busiType);
	if (itBusiType == sys.busis.end())
	{
		return;
	}

	// �Ҳ���Ӫҵ������
	BusinessType& bt = itBusiType->second;
	std::map<std::string, ConnectPool*>::iterator itBranch;
	bool bFoundBranch = false;
	for (itBranch = bt.connPool.begin(); itBranch != bt.connPool.end(); itBranch++)
	{
		std::string branchList = itBranch->first;

		if (branchList == "0000")
		{
			bFoundBranch = true;
			break;
		}

		if (branchList.find(sBranchId) != std::string::npos)
		{
			bFoundBranch = true;
			break;
		}
		else
		{
			bFoundBranch = false;
		}
	}

	if (!bFoundBranch)
	{
		return;
	}

	
	ConnectPool * pool = itBranch->second;
	pool->PushConnect(pConn);

}

void CConnectManager::CloseConnPool()
{
	std::map<std::string, BusinessSystem>::iterator itSys;


	for (itSys = systems.begin(); itSys != systems.end(); itSys++)
	{
		BusinessSystem& bs = itSys->second;

		std::map<int, BusinessType>::iterator itBusiType;
		for (itBusiType = bs.busis.begin(); itBusiType != bs.busis.end(); itBusiType++)
		{
			BusinessType& bt = itBusiType->second;

			std::map<std::string, ConnectPool*>::iterator itBranch;
			for (itBranch = bt.connPool.begin(); itBranch != bt.connPool.end(); itBranch++)
			{
				ConnectPool * pConnPool = itBranch->second;
				if (pConnPool != NULL && pConnPool->IsCreatePool())
				{
					pConnPool->CloseConnPool();
					delete pConnPool;
					pConnPool = NULL;
				}
			}
		}
	}
}
*/

// ����ϵͳ��š�ҵ�����͡�Ӫҵ�����ҵ���һ����̨��Ϣ
Counter* CConnectManager::GetNextCounter(std::string sysNo, int busiType, std::string sBranchId)
{
	// �˺�����Ҫ��ͬ��mutex

	// �Ҳ���ϵͳ��ֱ�ӷ���
	std::map<std::string, BusinessSystem>::iterator itSys;
	itSys = systems.find(sysNo);
	if (itSys == systems.end())
		return NULL;

	// �Ҳ���ҵ�����ͷ���
	BusinessSystem& sys = itSys->second;
	std::map<int, BusinessType >::iterator itBusiType;
	itBusiType = sys.busis.find(busiType);
	if (itBusiType == sys.busis.end())
	{
		return NULL;
	}

	// �Ҳ���Ӫҵ������
	BusinessType& bt = itBusiType->second;
	std::map<std::string, Branch>::iterator itBranch;
	bool bFoundBranch = false;
	for (itBranch = bt.branches.begin(); itBranch != bt.branches.end(); itBranch++)
	{
		std::string branchList = itBranch->first;

		if (branchList == "0000")
		{
			bFoundBranch = true;
			break;
		}

		if (branchList.find(sBranchId) != std::string::npos)
		{
			bFoundBranch = true;
			break;
		}
		else
		{
			bFoundBranch = false;
		}
	}

	if (!bFoundBranch)
	{
		return NULL;
	}

	
	Branch& branch = itBranch->second;

	return branch.GetNextCounter();
}

// ����ϵͳ��ţ�ҵ�����ͣ�Ӫҵ�����ҵ���Ӧ�����õĹ�̨����������
int CConnectManager::GetCounterCount(std::string sysNo, int busiType, std::string sBranchId)
{
		// �˺�����Ҫ��ͬ��mutex

	// �Ҳ���ϵͳ��ֱ�ӷ���
	std::map<std::string, BusinessSystem>::iterator itSys;
	itSys = systems.find(sysNo);
	if (itSys == systems.end())
		return 0;

	// �Ҳ���ҵ�����ͷ���
	BusinessSystem& sys = itSys->second;
	std::map<int, BusinessType >::iterator itBusiType;
	itBusiType = sys.busis.find(busiType);
	if (itBusiType == sys.busis.end())
	{
		return 0;
	}

	// �Ҳ���Ӫҵ������
	BusinessType& bt = itBusiType->second;
	std::map<std::string, Branch>::iterator itBranch;
	bool bFoundBranch = false;
	for (itBranch = bt.branches.begin(); itBranch != bt.branches.end(); itBranch++)
	{
		std::string branchList = itBranch->first;

		if (branchList == "0000")
		{
			bFoundBranch = true;
			break;
		}

		if (branchList.find(sBranchId) != std::string::npos)
		{
			bFoundBranch = true;
			break;
		}
		else
		{
			bFoundBranch = false;
		}
	}

	if (!bFoundBranch)
	{
		return 0;
	}

	
	Branch& branch = itBranch->second;

	return branch.GetCounterCount();
}


// ����ϵͳ��ź�ҵ�����ͣ��ҵ���Ӧ�Ĺ�̨����
int CConnectManager::GetCounterType(std::string SystemNo, std::string busiType)
{
	int bt = boost::lexical_cast<int>(busiType);

	std::map<std::string, BusinessSystem>::iterator it;
	it = systems.find(SystemNo);
	if (it == systems.end())
	{
		return COUNTER_TYPE_UNKNOWN;
	}

	BusinessSystem& bs = it->second;

	

	std::map<int, BusinessType>::iterator it2;

	
	it2 = bs.busis.find(bt);
	if (it2 == bs.busis.end())
	{
		return COUNTER_TYPE_UNKNOWN;
	}

	int ct = it2->second.counterType;
	return ct;
}
