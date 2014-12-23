#ifndef TRADE_BUSINESS_DINGDIAN_H
#define TRADE_BUSINESS_DINGDIAN_H

#include <string>
#include <map>



#include "IBusiness.h"
#include "DingDian.h"


class TradeBusinessDD : public IBusiness
{
public:
	TradeBusinessDD();
	TradeBusinessDD(int connId, Counter counter);
	~TradeBusinessDD(void);

	virtual bool CreateConnect();
	virtual void CloseConnect();
	virtual bool Send(std::string& request, std::string& response, int& status, std::string& errCode, std::string& errMsg);


private:
	char* EncryptPwd(const char* plainText, char* szPwd);
	long m_hHandle;
	void GetMacAndDiskID(std::string hardinfo, std::string& mac, std::string& diskid);

	
	CDingDian dd; 
};
#endif
