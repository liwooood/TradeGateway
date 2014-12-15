#ifndef ISESSION_H
#define ISESSION_H



// ��֤
#include "TradeBusinessJZ.h"

// ����
#include "TradeBusinessHS.h"
// ����
#include "TradeBusinessDD.h"
// AGC
#include "TradeBusinessJSD.h"
// ����
//#include "TCPClientSync.h"


class IMessage;

class ISession
{
public:
	ISession(void);
	~ISession(void);

	
	virtual void write(IMessage* resp) = 0;

protected:
	// ���Ự������ģʽ
	TradeBusinessHS hs;
	TradeBusinessJZ jz;
	TradeBusinessDD dd;
	TradeBusinessJSD jsd;
	//CTCPClientSync test;

	// ��Ϣ����
	int msgType;

	virtual void start()=0;
	virtual void close()=0;

	virtual void read() = 0;
	virtual IMessage* CreateRequest() = 0;
	virtual void OnReadHead(const boost::system::error_code& error, size_t transferredBytes, IMessage* req) = 0;
	virtual void OnReadMsg(const boost::system::error_code& error, size_t transferredBytes, IMessage* req) = 0;
	
	
	virtual void OnWriteHead(const boost::system::error_code& error, size_t transferredBytes, IMessage* resp) = 0;
	virtual void OnWriteMsg(const boost::system::error_code& error, size_t transferredBytes, IMessage* resp) = 0;

	
	// �رչ�̨����
	void CloseCounterConnect();
	// �õ���̨����
	//IBusiness * GetCounterConnect(int counterType);
};

#endif
