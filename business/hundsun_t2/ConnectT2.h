#ifndef _CONNECT_
#define _CONNECT_

#include "stdafx.h"

#include <string>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "common.h"
#include "t2sdk_interface.h"
#include "connectpool/Counter.h"
#include "CallbackImpl.h"
#include "connectpool/IConnect.h"
#include "./business/IBusiness.h"


class ConnectT2 : public IConnect, public IBusiness
{
private:
	int m_nID;
	Counter m_Counter;
	std::string m_sServerInfo;
	int connectTimeout;

	CConfigInterface * lpConfig;
	CConnectionInterface * lpConnection;

	CCallbackImpl callback;
	// 连接中断事件
	HANDLE hCloseEvent;
	// 应答事件
	HANDLE hResponseEvent;

private:
	static DWORD WINAPI AutoConnect(LPVOID lpParam);


public:
	ConnectT2(int ConnectNo, Counter counter);
	~ConnectT2(void);


	virtual std::string GetConnectInfo();
	virtual bool CreateConnect();
	virtual void CloseConnect();

	//virtual void * GetCounterConnect();
	//virtual void SendRequest(std::string request);
	virtual HANDLE GetResponseEvent();
	virtual void WaitResponseEvent();
	virtual std::string GetResponse();

	//bool ReConnect();
	

	//CCallbackImpl * get();
	// 继承自IBusiness
	virtual bool Send(std::string& request, std::string& response, int& status, std::string& errCode, std::string& errMsg);
};
#endif
