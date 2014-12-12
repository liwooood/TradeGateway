#include "stdafx.h"

#include "ISession.h"





ISession::ISession(void)
{
	
}


ISession::~ISession(void)
{
	
}


void ISession::CloseCounterConnect()
{
	/*
	if (counterT2 != NULL)
	{
		counterT2->CloseConnect();
		
	}

	if (counterSzkingdom != NULL)
	{
		counterSzkingdom->CloseConnect();
		
	}

	if (counterApex != NULL)
	{
		counterApex->CloseConnect();
		
	}

	if (counterAGC != NULL)
	{
		counterAGC->CloseConnect();
		
	}

	if (counterXinYi != NULL)
	{
		counterXinYi->CloseConnect();
		
	}
	*/
}
/*
// 根据参数，返回对应的柜台连接
IBusiness * ISession::GetCounterConnect(int counterType)
{

	IBusiness * business = NULL;

	switch(counterType)
	{
	case COUNTER_TYPE_HS_T2:
		business = counterT2;
		break;
	case COUNTER_TYPE_HS_COM:
		
		break;
	case COUNTER_TYPE_JZ_WIN:
		business = counterSzkingdom;
		break;
	case COUNTER_TYPE_JZ_LINUX:
		
		break;
	case COUNTER_TYPE_DINGDIAN:
		business = counterApex;
		break;
	case COUNTER_TYPE_JSD:
		business = counterAGC;
		break;
	case COUNTER_TYPE_XINYI:
		business = counterXinYi;
		break;
	
	}

	return business;
}
*/