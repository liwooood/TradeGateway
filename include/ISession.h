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

	// 柜台连接
	TradeBusinessHS hs;
	TradeBusinessJZ jz;
	TradeBusinessDD dd;
	TradeBusinessJSD jsd;
	//CTCPClientSync test;

	// 消息类型
	int m_msgType;

	virtual void start()=0;
	virtual void close()=0;
	
	virtual IMessage* create_request() = 0;
	virtual void read() = 0;
	virtual void handle_read_head(const boost::system::error_code& error, size_t bytes_transferred, IMessage* req) = 0;
	virtual void handle_read_msg(const boost::system::error_code& error, size_t bytes_transferred, IMessage* req) = 0;
	
	virtual void write(IMessage* resp) = 0;
	virtual void handle_write_head(const boost::system::error_code& error, size_t bytes_transferred, IMessage* resp) = 0;
	virtual void handle_write_msg(const boost::system::error_code& error, size_t bytes_transferred, IMessage* resp) = 0;

	
	// 关闭柜台连接
	void CloseCounterConnect();
	// 得到柜台连接
	//IBusiness * GetCounterConnect(int counterType);
};

#endif
