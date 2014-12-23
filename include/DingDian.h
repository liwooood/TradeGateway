#ifndef DINGDIAN_H
#define DINGDIAN_H

#include <map>
#include <string>
#include <fstream>
#include <iostream>

class CDingDian
{
public:
	CDingDian(void);
	~CDingDian(void);

	BOOL m_bIsInited;

	// FID_KHH, 605
	std::map<std::string, long> m_mRequestField;

	// ¹¦ÄÜºÅ, [FID_CODE, 507]
	std::map<std::string, std::map<std::string, long>> m_mReturnField;

	void ReadMapFile();
	void ReadOutDomainFile();

	BOOL Init();
	void UnInit();


};
extern CDingDian g_DingDian;

#endif
