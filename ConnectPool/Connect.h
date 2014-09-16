#pragma once
#include <string>

class CConnect
{
public:
	virtual bool CreateConnect() = 0;
	virtual void CloseConnect() = 0;
	virtual std::string GetConnectInfo() = 0;
};

