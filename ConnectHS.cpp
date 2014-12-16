#include "ConnectHS.h"


ConnectHS::ConnectHS(int connId, Counter counter)
{
	this->connId = connId;
	this->counter = counter;
}


ConnectHS::~ConnectHS(void)
{
}

bool ConnectHS::CreateConnect()
{
	return false;
}

void ConnectHS::CloseConnect()
{
}