#ifndef ICONNECT_H
#define ICONNECT_H



#include <string>

#include "common.h"
#include "Counter.h"


class IConnect
{
public:
	void SetCounter(Counter counter);
	Counter& GetCounter();

	
	std::string GetConnectInfo();

	

protected:
	
	/*
	//virtual void * GetCounterConnect() = 0;
	virtual bool Send(std::string request) = 0;
	virtual bool Send(std::string& request, std::string& response, int& status, std::string& errCode, std::string& errMsg) = 0;
	virtual DWORD WaitResponseEvent() = 0;
	virtual T2_ASYNC_RET GetSendResponse() = 0;
	virtual T2_ASYNC_RET GetAsyncResponse() = 0;
	*/
};

#endif
