#pragma once
#include "stdafx.h"

#include <string>

class IConnect
{
public:
	virtual bool CreateConnect() = 0;
	virtual void CloseConnect() = 0;
	virtual std::string GetConnectInfo() = 0;

	//virtual void * GetCounterConnect() = 0;
	
	

	virtual bool Send(std::string& request, std::string& response, int& status, std::string& errCode, std::string& errMsg) = 0;

	virtual void WaitResponseEvent() = 0;
	virtual HANDLE GetResponseEvent() = 0;

	virtual std::string GetResponse() = 0;
};

