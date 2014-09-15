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


class Connect
{
private:
	CConfigInterface * lpConfig;
	CConnectionInterface * lpConnection;
	int m_nID;
	Counter m_Counter;
	std::string m_sServerInfo;
	CCallbackImpl callback;


public:
	Connect(int ConnectNo, Counter counter);
	~Connect(void);

	void init();
	std::string GetConnectInfo();
	bool CreateConnect();
	bool ReConnect();
	void CloseConnect();

	//CCallbackImpl * get();
};
#endif
