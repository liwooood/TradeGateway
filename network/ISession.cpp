#include "stdafx.h"

#include "ISession.h"


// ��֤
#include "tradebusiness.h"

// ����
#include "tradebusinesst2.h"
// ����
#include "TradeBusinessDingDian.h"
#include "DingDian.h"
// AGC
#include "SywgConnect.h"
// ����
//#include "TCPClientSync.h"


ISession::ISession(void)
{
	counterT2 = NULL;
	counterSzkingdom = NULL;
	counterApex = NULL;
	counterAGC = NULL;
	counterXinYi = NULL;

	counterT2 = new TradeBusinessT2();
	counterSzkingdom = new TradeBusiness();
	counterApex = new TradeBusinessDingDian();
	counterAGC = new CSywgConnect();
//	counterXinYi = new CTCPClientSync();
}


ISession::~ISession(void)
{
	if (counterT2 != NULL)
	{
		delete counterT2;
		counterT2 = NULL;
	}

	if (counterSzkingdom != NULL)
	{
		delete counterSzkingdom;
		counterSzkingdom = NULL;
	}

	if (counterApex != NULL)
	{
		delete counterApex;
		counterApex = NULL;
	}

	if (counterAGC != NULL)
	{
		delete counterAGC;
		counterAGC = NULL;
	}

	if (counterXinYi != NULL)
	{
		delete counterXinYi;
		counterXinYi = NULL;
	}
}


void ISession::CloseCounterConnect()
{
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
}
/*
// ���ݲ��������ض�Ӧ�Ĺ�̨����
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