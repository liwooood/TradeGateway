#ifndef _BUSINESS_TYPE_
#define _BUSINESS_TYPE_

#include <string>
#include <map>

#include "common.h"
#include "Counter.h"

#include "branch.h"

class BusinessType
{
public:
	BusinessType(void);
	~BusinessType(void);

	
	int counterType;
	int asyncMode;

	//std::map<std::string, ConnectPool*> connPool;

	std::map<std::string/*Ӫҵ��id*/, Branch> branches;
};

#endif
