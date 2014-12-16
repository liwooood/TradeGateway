#ifndef COUNTER_H
#define COUNTER_H

#include <string>

#include "common.h"

//改用struct

class Counter
{
public:
	Counter(void);
	~Counter(void);

	int m_nCounterType; // 柜台类型
	std::string m_sIP;
	int m_nPort;
	int m_nConnectTimeout; // 柜台连接超时时间
	int m_nRecvTimeout; // 请求处理超时时间
	std::string entrustMode; // 委托方式
	

	//以下信息金证专用
	std::string m_sServerName;
	std::string m_sUserName;
	std::string m_sPassword;
	std::string m_sReq;
	std::string m_sRes;

	// 顶点柜台，柜员代码
	std::string m_sGydm; 


	
	//std::string m_sWtfs_mobile;
	//std::string m_sWtfs_web;

};
#endif
