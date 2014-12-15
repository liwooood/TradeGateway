#include "stdafx.h"

#include "ISession.h"





ISession::ISession(void)
{
	
}


ISession::~ISession(void)
{
	
}


// ���ݲ��������ض�Ӧ�Ĺ�̨����
IBusiness& ISession::GetCounterConnect(int counterType)
{

	if(counterType == COUNTER_TYPE_HS_T2)
	{
	
		return hs;
	}
	
	else if(counterType == COUNTER_TYPE_JZ_WIN)
	{
	
		return jz;
	}
	
	else if(counterType == COUNTER_TYPE_DINGDIAN)
	{
		return dd;
	}
	else if(counterType == COUNTER_TYPE_JSD)
	{
		return jsd;
	}
	else if(counterType == COUNTER_TYPE_XINYI)
	{
		return test;
	}
	else
	{
	}	
	
}


void ISession::CloseCounterConnect()
{
	hs.CloseConnect();
	jz.CloseConnect();
	dd.CloseConnect();
	jsd.CloseConnect();
	test.CloseConnect();
}
