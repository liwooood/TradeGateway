#include "stdafx.h"

#include "CustomMessage.h"


CustomMessage::CustomMessage(int msgType)
{
	int msgHeaderSize = sizeof(MSG_HEADER);

	msgHeader.resize(msgHeaderSize);

	msgType = msgType;
}



bool CustomMessage::DecoderMsgHeader()
{
	memcpy(&customMsgHeader, msgHeader.data(), msgHeader.size());
	
	// java����������c++����������������Ҫת��
	//int MsgContentSize = ntohl(m_MsgHeader.MsgContentSize);
	//m_MsgHeader.MsgContentSize = MsgContentSize;
	if (customMsgHeader.MsgContentSize >= 65536 || customMsgHeader.MsgContentSize <= 0)
		return false;

	msgContent.resize(customMsgHeader.MsgContentSize);

	return true;
}

