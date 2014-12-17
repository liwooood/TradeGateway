#ifndef TRADE_BUSINESS_TEST_H
#define TRADE_BUSINESS_TEST_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/array.hpp>
#include <boost/scoped_array.hpp> 
#include <boost/noncopyable.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>


#include "IBusiness.h"

#include "MsgHeader.h"
#include "CustomMessage.h"
/*
问题一：超时
问题二：循环读写
问题三：运行时间
*/
class TradeBusinessTest : public IBusiness
{
public:
	TradeBusinessTest();
	TradeBusinessTest(int connId, Counter counter);
	~TradeBusinessTest(void);

	virtual bool CreateConnect();
	virtual void CloseConnect();
	virtual bool Send(std::string& request, std::string& response, int& status, std::string& errCode, std::string& errMsg);
	
	
	

private:
	bool Write(CustomMessage * pReq);
	bool WriteMsgHeader(CustomMessage * pReq);
	bool WriteMsgContent(CustomMessage * pReq);

	bool Read(CustomMessage * pRes);
	bool ReadMsgHeader(CustomMessage * pRes);
	bool ReadMsgContent(CustomMessage * pRes);

	int Send(const char * buf, int len, int flags);
	int Recv(char* buf, int len, int flags);
	
private:
	SOCKET sockfd;
};
#endif
