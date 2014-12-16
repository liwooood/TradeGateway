#ifndef BUSINESS_TYPE_H
#define BUSINESS_TYPE_H

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

	//存放柜台连接
	//std::map<std::string/*营业部列表id*/, ConnectPool*> connPool;

	// 存放柜台信息
	std::map<std::string/*营业部列表id*/, Branch> branches;
};

#endif
