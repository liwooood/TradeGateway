#ifndef IBUSINESS_H
#define IBUSINESS_H

// stl
#include <string>
#include <map>
#include <vector>

// boost
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>


#include "common.h"
#include "tradelog.pb.h"
#include "Counter.h"
#include "IConnect.h"


class IBusiness
{
protected:
	// �������
	Counter counter;
	
	int connId; // �����ӳ��е��������

	// ��ͬ�Ĺ�̨��д��ͬ����־
	int counterType;
	std::string logFile;


	// ҵ�����
	std::string SOH;
	bool m_bConnected;
	Counter * m_Counter;


	std::string sysNo;

	int busiType;
	std::string bt;

	std::string sysVer;

	std::string branchId;

	std::string funcid;
	long lFuncId;

	std::string route;
	int nRoute;

	

	std::string account; // �ͻ��ţ�����������־�ļ�


	std::string note;

	std::string hardinfo;

	std::string cssweb_cacheFlag;

	std::map<std::string, std::string> reqmap;

	std::vector<std::string> keys;
	int num;

	
	IConnect* pConn;

public:
	IBusiness(void);


	// �������
	virtual bool CreateConnect() = 0;
	virtual void CloseConnect() = 0;
	virtual std::string GetConnectInfo();

	// ҵ�����
	virtual bool Send(std::string& request, std::string& response, int& status, std::string& errCode, std::string& errMsg) = 0;
	

	
	bool ParseRequest(std::string& request);
	
	
	virtual void SetCounter(Counter * counter);
	virtual Counter * GetCounter();
	virtual bool IsConnected();
	
	bool FilterRequestField(std::string& key);
	void RetNoRecordRes(std::string& response, int& status);
	void GenResponse(int nErrCode, std::string sErrMsg, std::string& response, int& status, std::string& errCode, std::string& errMsg);
	int ConvertIntToBusiType(int val);
};

#endif
