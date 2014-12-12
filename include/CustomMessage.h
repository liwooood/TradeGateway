#ifndef CUSTOM_MESSAGE_H
#define CUSTOM_MESSAGE_H

#include "IMessage.h"



#include "MsgHeader.h"

/*
独立出来的原因，
1.以后可以改由内存池分配
2.异步调用的时候适合放入队列
*/




class CustomMessage : public IMessage
{
public:
	CustomMessage(int msgType);

private:
	// 消息头
	struct MsgHeader customMsgHeader;
	

public:
	bool DecoderMsgHeader();
};



#endif