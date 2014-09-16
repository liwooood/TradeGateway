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
#include "connectpool/connect.h"


class ConnectT2 : public CConnect
{
private:
	CConfigInterface * lpConfig;
	CConnectionInterface * lpConnection;
	int m_nID;
	Counter m_Counter;
	std::string m_sServerInfo;
	CCallbackImpl callback;


public:
	ConnectT2(int ConnectNo, Counter counter);
	~ConnectT2(void);

	void init();
	virtual std::string GetConnectInfo();
	virtual bool CreateConnect();
	bool ReConnect();
	virtual void CloseConnect();

	//CCallbackImpl * get();
};
#endif
