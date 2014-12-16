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
	
	// ������ѯ�㷨����ȡ��һ����̨��������Ϣ
	Counter* GetNextCounter();

	int GetCounterCount();
};



#endif
