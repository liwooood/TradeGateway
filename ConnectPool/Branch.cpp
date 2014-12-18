#include "Branch.h"


Branch::Branch(void)
{
	currentServerId = 0;
	pool = NULL;
}


Branch::~Branch(void)
{
}

Counter* Branch::GetNextCounter()
{
	

	if (currentServerId >= servers.size())
		currentServerId = 0;

	Counter& counter = servers[currentServerId++];

	return &counter;
}

int Branch::GetCounterCount()
{
	return servers.size();
}
