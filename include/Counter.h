#ifndef COUNTER_H
#define COUNTER_H

#include <string>

#include "common.h"

//����struct

class Counter
{
public:
	Counter(void);
	~Counter(void);

	int m_nCounterType; // ��̨����
	std::string m_sIP;
	int m_nPort;
	int m_nConnectTimeout; // ��̨���ӳ�ʱʱ��
	int m_nRecvTimeout; // ������ʱʱ��
	std::string entrustMode; // ί�з�ʽ
	

	//������Ϣ��֤ר��
	std::string m_sServerName;
	std::string m_sUserName;
	std::string m_sPassword;
	std::string m_sReq;
	std::string m_sRes;

	// �����̨����Ա����
	std::string m_sGydm; 


	
	//std::string m_sWtfs_mobile;
	//std::string m_sWtfs_web;

};
#endif
