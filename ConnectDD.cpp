#include "ConnectDD.h"


ConnectDD::ConnectDD(int connId, Counter counter)
{
	this->connId = connId;
	this->counter = counter;
}


ConnectDD::~ConnectDD(void)
{
}

bool ConnectDD::CreateConnect()
{
	return false;
}

void ConnectDD::CloseConnect()
{
}