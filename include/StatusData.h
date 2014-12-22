#ifndef STATUS_DATA_H
#define STATUS_DATA_H

#include "boost/atomic.hpp"

class StatusData
{
public:
	StatusData(void);
	~StatusData(void);

	// �Ự��
	boost::atomic_long sslOldSessions;
	boost::atomic_long sslOldMaxSessions;

	boost::atomic_long tcpOldSessions;
	boost::atomic_long tcpOldMaxSessions;

	boost::atomic_long sslNewSessions;
	boost::atomic_long sslNewMaxSessions;

	boost::atomic_long tcpNewSessions;
	boost::atomic_long tcpNewMaxSessions;


	// ��¼�����ͳ�ƣ�ֻ�����û�Ψһ��ʶΪkey
	/*
	�ʽ��˺ţ��ɶ������¼�ɹ��󣬷��ؿͻ���
	��Ҫ�̰߳�ȫ��������¼�ѵ�¼�ͻ���

	*/
	boost::atomic_long loginUsers;
	boost::atomic_long maxLoginUsers;

};

extern StatusData gStatusData;

#endif
