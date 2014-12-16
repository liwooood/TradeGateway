#ifndef CONNECT_TEST_H
#define CONNECT_TEST_H

#include "IConnect.h"

class ConnectTest : public IConnect
{
public:
	ConnectTest(int connId, Counter counter);
	~ConnectTest(void);

	virtual bool CreateConnect();
	virtual void CloseConnect();
};

#endif
