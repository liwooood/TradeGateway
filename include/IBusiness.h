#ifndef _IBUSINESS_H_
#define _IBUSINESS_H_

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

class IBusiness
{
protected:
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

	

	std::string account; // 客户号，用于命名日志文件


	std::string note;

	std::string hardinfo;

	std::string cssweb_cacheFlag;

	std::map<std::string, std::string> reqmap;

	std::vector<std::string> keys;
	int num;

public:
	IBusiness(void);
	

	

	
	virtual void SetCounter(Counter * counter);
	virtual Counter * GetCounter();
	virtual bool IsConnected();


	virtual bool CreateConnect() = 0;
	virtual void CloseConnect() = 0;
	virtual bool Send(std::string& request, std::string& response, int& status, std::string& errCode, std::string& errMsg) = 0;


	bool ParseRequest(std::string& request);
	bool FilterRequestField(std::string& key);
	void RetNoRecordRes(std::string& response, int& status);
	void GenResponse(int nErrCode, std::string sErrMsg, std::string& response, int& status, std::string& errCode, std::string& errMsg);
	

	int ConvertIntToBusiType(int val);
};

#endif
