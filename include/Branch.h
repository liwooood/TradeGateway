#ifndef BRANCH_H
#define BRANCH_H

// stl
#include <map>
#include <vector>
#include <mutex>



#include "Counter.h"
#include "ConnectPool.h"



class Branch
{
public:
	Branch(void);
	~Branch(void);
	
	std::vector<Counter> servers;
	ConnectPool* pool;



	int currentServerId;
	
	// 采用轮询算法，获取下一个柜台服务器信息
	Counter* GetNextCounter();

	int GetCounterCount();
};



#endif
