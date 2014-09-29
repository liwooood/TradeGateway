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


class ConnectT2 : public IConnect
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

	virtual void * GetCounterConnect();
	virtual HANDLE GetResponseEvent();
	virtual std::string GetResponse();

	//bool ReConnect();
	

	//CCallbackImpl * get();
};
#endif
