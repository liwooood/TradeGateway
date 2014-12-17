#include "ConnectTest.h"


ConnectTest::ConnectTest(int connId, Counter counter)
{
	this->connId = connId;
	this->counter = counter;
}


ConnectTest::~ConnectTest(void)
{
}


SOCKET ConnectTest::GetConnect()
{
	return sockfd;
}