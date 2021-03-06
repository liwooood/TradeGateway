#include "stdafx.h"

#include <boost/lexical_cast.hpp>

#include "ConnectManager.h"
#include "ConfigManager.h"
#include "FileLog.h"
#include "ConnectPool.h"




CConnectManager g_ConnectManager;

CConnectManager::CConnectManager(void)
{
}

CConnectManager::~CConnectManager(void)
{
}

IBusiness* CConnectManager::GetConnect(std::string sysNo, int busiType, std::string sBranchId)
{
	IBusiness* pConn = NULL;

	// 找不到系统，直接返回
	std::map<std::string, BusinessSystem>::iterator itSys = systems.find(sysNo);
	if (itSys == systems.end())
		return NULL;

	// 找不到业务类型返回
	BusinessSystem& sys = itSys->second;

	std::map<int, BusinessType>::iterator itBusiType = sys.busis.find(busiType);
	if (itBusiType == sys.busis.end())
	{
		return NULL;
	}

	// 找不到营业部返回
	BusinessType& bt = itBusiType->second;
	bool bFoundBranch = false;

	std::map<std::string, Branch>::iterator itBranch;
	
	for (itBranch = bt.branches.begin(); itBranch != bt.branches.end(); itBranch++)
	{
		std::string branchList = itBranch->first;

		if (branchList.compare("0000") == 0)
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

	// 请求入参中的营业部在配置信息中找不到
	if (!bFoundBranch)
	{
		return NULL;
	}

	
	Branch& branch = itBranch->second;
	ConnectPool* pool = branch.pool;
	
	if (pool->IsCreatePoolSuccess())
	{
		pConn = pool->GetConnect();

	}
	else
	{
		if (!pool->CreateConnPool())
		{

			return NULL;
		}
		
		
		pConn = pool->GetConnect();
				
	}

	return pConn;
}

void CConnectManager::PushConnect(IBusiness * pConn, std::string sysNo, int busiType, std::string sBranchId)
{
	if (pConn == NULL)
		return;

	if (sysNo.empty())
		return ;


	// 找不到系统，直接返回
	std::map<std::string, BusinessSystem>::iterator itSys = systems.find(sysNo);
	if (itSys == systems.end())
		return;
	BusinessSystem& sys = itSys->second;


	// 找不到业务类型返回
	std::map<int, BusinessType >::iterator itBusiType = sys.busis.find(busiType);
	if (itBusiType == sys.busis.end())
	{
		return;
	}
	BusinessType& bt = itBusiType->second;


	// 找不到营业部返回
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
		return;
	}

	Branch& branch = itBranch->second;

	ConnectPool * pool = branch.pool;
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

			std::map<std::string, Branch>::iterator itBranch;
			for (itBranch = bt.branches.begin(); itBranch != bt.branches.end(); itBranch++)
			{
				Branch& branch = itBranch->second;
				ConnectPool * pConnPool = branch.pool;

				if (pConnPool != NULL && pConnPool->IsCreatePoolSuccess())
				{
					pConnPool->CloseConnPool();
					delete pConnPool;
					pConnPool = NULL;
				}
			}
		}
	}
}


	/*
// 根据系统编号、业务类型、营业部，找到下一个柜台信息
Counter* CConnectManager::GetNextCounter(std::string sysNo, int busiType, std::string sBranchId)
{
	// 此函数需要加同步mutex

	// 找不到系统，直接返回
	std::map<std::string, BusinessSystem>::iterator itSys;
	itSys = systems.find(sysNo);
	if (itSys == systems.end())
		return NULL;

	// 找不到业务类型返回
	BusinessSystem& sys = itSys->second;
	std::map<int, BusinessType >::iterator itBusiType;
	itBusiType = sys.busis.find(busiType);
	if (itBusiType == sys.busis.end())
	{
		return NULL;
	}

	// 找不到营业部返回
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

// 根据系统编号，业务类型，营业部，找到对应的配置的柜台服务器数量
int CConnectManager::GetCounterCount(std::string sysNo, int busiType, std::string sBranchId)
{
		// 此函数需要加同步mutex

	// 找不到系统，直接返回
	std::map<std::string, BusinessSystem>::iterator itSys;
	itSys = systems.find(sysNo);
	if (itSys == systems.end())
		return 0;

	// 找不到业务类型返回
	BusinessSystem& sys = itSys->second;
	std::map<int, BusinessType >::iterator itBusiType;
	itBusiType = sys.busis.find(busiType);
	if (itBusiType == sys.busis.end())
	{
		return 0;
	}

	// 找不到营业部返回
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
*/

// 根据系统编号和业务类型，找到对应的柜台类型
bool CConnectManager::GetCounterTypeAndAsyncMode(std::string& systemNo, std::string& businessType, int& counterType, int& counterMode)
{
	

	std::map<std::string, BusinessSystem>::iterator it;
	it = systems.find(systemNo);
	if (it == systems.end())
	{
		return false;
	}

	BusinessSystem& bs = it->second;

	

	std::map<int, BusinessType>::iterator it2;

	int bt = boost::lexical_cast<int>(businessType);
	it2 = bs.busis.find(bt);
	if (it2 == bs.busis.end())
	{
		return false;
	}

	counterType = it2->second.counterType;
	
	counterMode = it2->second.counterMode;

	return true;
}
