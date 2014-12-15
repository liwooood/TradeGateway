#ifndef TRADE_BUSINESS_T2_H
#define TRADE_BUSINESS_T2_H

#include <string>
#include <map>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "tradelog.pb.h"
#include "t2sdk_interface.h"
#include "IConnect.h"
#include "IBusiness.h"


class TradeBusinessHS : public IConnect, public IBusiness
{
private:
	int m_nID;
	Counter m_Counter;
	std::string m_sServerInfo;
	int connectTimeout;
	int readWriteTimeout;

	CConfigInterface * lpConfig;
	CConnectionInterface * lpConnection;
	

public:
	TradeBusinessHS();
	TradeBusinessHS(int ConnectNo, Counter counter);
	

	virtual bool CreateConnect();
	virtual void CloseConnect();
	virtual std::string GetConnectInfo()
	{
		return m_sServerInfo;
	}

	
	virtual bool Send(std::string& request, std::string& response, int& status, std::string& errCode, std::string& errMsg);
	virtual bool Send(std::string request)
	{
		return false;
	};

	virtual DWORD WaitResponseEvent()
	{
		return 0;
	};

	virtual T2_ASYNC_RET GetSendResponse()
	{
		T2_ASYNC_RET ret;
		return ret;
	};
	

	virtual T2_ASYNC_RET GetAsyncResponse()
	{
		T2_ASYNC_RET ret;
		return ret;
	};
};
#endif
