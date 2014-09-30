#ifndef _ISESSION_H_
#define _ISESSION_H_

#include <boost/asio.hpp>

#include "IBusiness.h"
//#include "network/CustomMessage.h"

class IMessage;

class ISession
{
public:
	ISession(void);
	~ISession(void);

	// ��̨����
	IBusiness * counterT2;
	IBusiness * counterSzkingdom;
	IBusiness * counterApex;
	IBusiness * counterAGC;
	IBusiness * counterXinYi;

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
	IBusiness * GetCounterConnect(int counterType);
};

#endif
