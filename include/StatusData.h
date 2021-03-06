#ifndef STATUS_DATA_H
#define STATUS_DATA_H

#include "boost/atomic.hpp"

class StatusData
{
public:
	StatusData(void);
	~StatusData(void);

	// 会话数
	boost::atomic_long sslOldSessions;
	boost::atomic_long sslOldMaxSessions;

	boost::atomic_long tcpOldSessions;
	boost::atomic_long tcpOldMaxSessions;

	boost::atomic_long sslNewSessions;
	boost::atomic_long sslNewMaxSessions;

	boost::atomic_long tcpNewSessions;
	boost::atomic_long tcpNewMaxSessions;


	// 登录数如何统计，只能以用户唯一标识为key
	/*
	资金账号，股东代码登录成功后，返回客户号
	需要线程安全的容器记录已登录客户号

	*/
	boost::atomic_long loginUsers;
	boost::atomic_long maxLoginUsers;

};

extern StatusData gStatusData;

#endif
