#include "ConnectJSD.h"


ConnectJSD::ConnectJSD(int connId, Counter counter)
{
	this->connId = connId;
	this->counter = counter;
}

ConnectJSD::~ConnectJSD(void)
{
}

bool ConnectJSD::CreateConnect()
{
	return false;
}

void ConnectJSD::CloseConnect()
{
}