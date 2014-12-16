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

	// 柜台类型
	int counterType;

	// 柜台模式，会话模式或连接池模式
	int counterMode; 

	

	// 存放柜台信息
	std::map<std::string/*营业部列表id*/, Branch> branches;
};

#endif
