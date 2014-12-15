#ifndef ISESSION_H
#define ISESSION_H



// 金证
#include "TradeBusinessJZ.h"

// 恒生
#include "TradeBusinessHS.h"
// 顶点
#include "TradeBusinessDD.h"
// AGC
#include "TradeBusinessJSD.h"
// 新意
//#include "TCPClientSync.h"


class IMessage;

class ISession
{
public:
	ISession(void);
	~ISession(void);

	
	virtual void write(IMessage* resp) = 0;

protected:
	// 单会话单连接模式
	TradeBusinessHS hs;
	TradeBusinessJZ jz;
	TradeBusinessDD dd;
	TradeBusinessJSD jsd;
	//CTCPClientSync test;

	// 消息类型
	int msgType;

	virtual void start()=0;
	virtual void close()=0;

	virtual void read() = 0;
	virtual IMessage* CreateRequest() = 0;
	virtual void OnReadHead(const boost::system::error_code& error, size_t transferredBytes, IMessage* req) = 0;
	virtual void OnReadMsg(const boost::system::error_code& error, size_t transferredBytes, IMessage* req) = 0;
	
	
	virtual void OnWriteHead(const boost::system::error_code& error, size_t transferredBytes, IMessage* resp) = 0;
	virtual void OnWriteMsg(const boost::system::error_code& error, size_t transferredBytes, IMessage* resp) = 0;

	
	// 关闭柜台连接
	void CloseCounterConnect();
	// 得到柜台连接
	//IBusiness * GetCounterConnect(int counterType);
};

#endif
