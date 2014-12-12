#ifndef TCP_MESSAGE_OLD_H
#define TCP_MESSAGE_OLD_H


#include "stdafx.h"




#include "imessage.h"

class TcpMessageOld : public IMessage
{
public:
	TcpMessageOld()
	{
		msgHeader.resize(4);

		msgType = MSG_TYPE_TCP_OLD;
	}

	
	virtual bool DecoderMsgHeader()
	{
		int msgContentSize = 0;

		memcpy(&msgContentSize, msgHeader.data(), msgHeader.size());

		msgContentSize = ntohl(msgContentSize);

		if (msgContentSize <= 0 || msgContentSize >= 4096 )
		{
			return false;
		}

		msgContent.resize(msgContentSize);

		return true;
	}
};


#endif
