#pragma once
#include "stdafx.h"

#include <string>

class CConnect
{
public:
	virtual bool CreateConnect() = 0;
	virtual void CloseConnect() = 0;
	virtual std::string GetConnectInfo() = 0;

	virtual void * GetCounterConnect() = 0;
	virtual HANDLE GetResponseEvent() = 0;
	virtual std::string GetResponse() = 0;
};

