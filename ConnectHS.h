#ifndef CONNECT_HS_H
#define CONNECT_HS_H

#include "IConnect.h"
#include "t2sdk_interface.h"

class ConnectHS : public IConnect
{
public:
	ConnectHS(int connId, Counter counter);
	~ConnectHS(void);

	virtual bool CreateConnect();
	virtual void CloseConnect();

private:
	CConfigInterface * lpConfig;
	CConnectionInterface * lpConnection;

};

#endif