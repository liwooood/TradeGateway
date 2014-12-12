#ifndef SSL_MESSAGE_PB_H
#define SSL_MESSAGE_PB_H

#include "imessage.h"

#include "pkgheader.pb.h"


class SslMessagePB : public IMessage
{
public:
	
	SslMessagePB()
	{	
		msgHeader.resize(23);

		msgType = MSG_TYPE_SSL_PB;
	}

	virtual bool DecoderMsgHeader()
	{
		if (!pbHeader.ParseFromArray(msgHeader.data(), msgHeader.size()))
		{
			return false;
		}

		msgContent.resize(pbHeader.bodysize());

		return true;
	}

private:
	quote::PkgHeader pbHeader;


};

#endif
