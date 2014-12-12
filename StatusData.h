#ifndef STATUS_DATA_H
#define STATUS_DATA_H

#include "boost/atomic.hpp"

class StatusData
{
public:
	StatusData(void);
	~StatusData(void);

	// �Ự��
	boost::atomic_long sessions;
	boost::atomic_long maxSessions;

	// ��¼�����ͳ�ƣ�ֻ�����û�Ψһ��ʶΪkey
	/*
	�ʽ��˺ţ��ɶ������¼�ɹ��󣬷��ؿͻ���
	��Ҫ�̰߳�ȫ��������¼�ѵ�¼�ͻ���

	*/
	boost::atomic_long loginUsers;
	boost::atomic_long maxLoginUsers;

};

#endif
