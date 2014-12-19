#ifndef TRADE_SERVER_H
#define TRADE_SERVER_H

#include "CustomMessage.h"
#include "QueueThreadSafe.h"
#include "WorkerThreadPool.h"

//#include "network/TcpSession.h"

#include "common.h"


class TradeServer
{
public:
	typedef QueueThreadSafe<IMessage*> req_queue_type;
	typedef WorkerThreadPool<req_queue_type> req_worker_type;

	typedef QueueThreadSafe<IMessage*> resp_queue_type;
	typedef WorkerThreadPool<resp_queue_type> resp_worker_type;

private:
	req_queue_type recvq_;
	resp_queue_type sendq_;

	req_worker_type req_worker_;
	resp_worker_type resp_worker_;


public:
	TradeServer(int msgType);
	req_queue_type& recv_queue();
	void start();
	void stop();

private:
	// 处理请求消息
	bool ProcessRequest(IMessage* req);

	// 处理应答消息
	bool ProcessResponse(IMessage* resp);


	bool GetSysNoAndBusiType(std::string& request, std::string& sysNo, std::string& busiType, std::string& sysVer, std::string& account, std::string& funcId, std::string& errCode, std::string& errMsg);


	
	int m_MsgType;



public:
	void set();

};
#endif
