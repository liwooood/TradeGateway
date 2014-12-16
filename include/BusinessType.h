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

	// ��̨����
	int counterType;

	// ��̨ģʽ���Ựģʽ�����ӳ�ģʽ
	int counterMode; 

	

	// ��Ź�̨��Ϣ
	std::map<std::string/*Ӫҵ���б�id*/, Branch> branches;
};

#endif
