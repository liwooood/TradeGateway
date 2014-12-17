#ifndef CONNECT_TEST_H
#define CONNECT_TEST_H

#include <WinSock2.h>

#include "IConnect.h"

class ConnectTest : public IConnect
{
public:
	ConnectTest(int connId, Counter counter);
	~ConnectTest(void);

	virtual bool CreateConnect();
	virtual void CloseConnect();

	SOCKET GetConnect();

private:
	

};

#endif
