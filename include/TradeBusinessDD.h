#ifndef TRADE_BUSINESS_DINGDIAN_H
#define TRADE_BUSINESS_DINGDIAN_H

#include <string>
#include <map>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "tradelog.pb.h"
#include "IBusiness.h"


class TradeBusinessDD : public IBusiness
{
public:
	TradeBusinessDD();
	~TradeBusinessDD(void);

	virtual bool CreateConnect();
	virtual void CloseConnect();
	virtual bool Send(std::string& request, std::string& response, int& status, std::string& errCode, std::string& errMsg);


private:
	char* EncryptPwd(const char* plainText, char* szPwd);
	long m_hHandle;
	void GetMacAndDiskID(std::string hardinfo, std::string& mac, std::string& diskid);
};
#endif
