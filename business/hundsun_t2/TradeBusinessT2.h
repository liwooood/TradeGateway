#include <string>
#include <map>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "./log/tradelog.pb.h"
#include "business/hundsun_t2/t2sdk_interface.h"
#include "./business/IBusiness.h"


class TradeBusinessT2 : public IBusiness
{
private:
	CConfigInterface * lpConfig;
	CConnectionInterface * lpConnection;

public:
	TradeBusinessT2();
	~TradeBusinessT2(void);

	virtual bool CreateConnect();
	virtual void CloseConnect();
	virtual bool Send(std::string& request, std::string& response, int& status, std::string& errCode, std::string& errMsg);
};
