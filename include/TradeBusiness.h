#ifndef _TRADE_BUSINESS_H_
#define _TRADE_BUSINESS_H_

#include <string>
#include <map>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


#include "IBusiness.h"


class TradeBusiness : public IBusiness
{
public:
	TradeBusiness();
	~TradeBusiness(void);

	virtual bool CreateConnect();
	virtual void CloseConnect();
	virtual bool Send(std::string& request, std::string& response, int& status, std::string& errCode, std::string& errMsg);
	


	// ��֤
	void * handle; // ��̨���Ӿ��
	
};
#endif
