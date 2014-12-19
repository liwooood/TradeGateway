#ifndef TRADE_BUSINESS_T2_H
#define TRADE_BUSINESS_T2_H

#include <string>
#include <map>


#include "t2sdk_interface.h"

#include "IBusiness.h"


class TradeBusinessHS : public IBusiness
{
private:



	CConfigInterface * lpConfig;
	CConnectionInterface * lpConnection;
	

public:
	TradeBusinessHS();
	TradeBusinessHS(int connId, Counter counter);
	

	virtual bool CreateConnect();
	virtual void CloseConnect();
	virtual bool Send(std::string& request, std::string& response, int& status, std::string& errCode, std::string& errMsg);
	
};
#endif
