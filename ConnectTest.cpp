#include "ConnectTest.h"


ConnectTest::ConnectTest(int connId, Counter counter)
{
	this->connId = connId;
	this->counter = counter;
}


ConnectTest::~ConnectTest(void)
{
}

bool ConnectTest::CreateConnect()
{
	return false;
}

void ConnectTest::CloseConnect()
{
}
