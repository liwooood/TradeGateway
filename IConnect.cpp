#include "stdafx.h"

#include "IConnect.h"

void IConnect::SetCounter(Counter counter)
{
	this->counter = counter;
}


Counter& IConnect::GetCounter()
{
	return counter;
}

std::string IConnect::GetConnectInfo()
{
	return "";
}
