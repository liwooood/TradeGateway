#include "ConnectJZ.h"


ConnectJZ::ConnectJZ(int connId, Counter counter)
{
	this->connId = connId;
	this->counter = counter;
}

ConnectJZ::~ConnectJZ(void)
{
}


bool ConnectJZ::CreateConnect()
{
	return false;
}

void ConnectJZ::CloseConnect()
{
}
