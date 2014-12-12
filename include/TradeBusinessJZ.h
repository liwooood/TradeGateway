#ifndef _TRADE_BUSINESS_H_
#define _TRADE_BUSINESS_H_

#include <string>
#include <map>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


#include "IBusiness.h"


class TradeBusinessJZ : public IBusiness
{
public:
	TradeBusinessJZ();
	~TradeBusinessJZ(void);

	virtual bool CreateConnect();
	virtual void CloseConnect();
	virtual bool Send(std::string& request, std::string& response, int& status, std::string& errCode, std::string& errMsg);
	


	// 金证
	void * handle; // 柜台连接句柄
	
};
#endif
