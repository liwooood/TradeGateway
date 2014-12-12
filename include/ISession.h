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

	// ��̨����
	TradeBusinessHS hs;
	TradeBusinessJZ jz;
	TradeBusinessDD dd;
	TradeBusinessJSD jsd;
	//CTCPClientSync test;

	// ��Ϣ����
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

	
	// �رչ�̨����
	void CloseCounterConnect();
	// �õ���̨����
	//IBusiness * GetCounterConnect(int counterType);
};

#endif
