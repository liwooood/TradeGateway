#ifndef CUSTOM_MESSAGE_H
#define CUSTOM_MESSAGE_H

#include "IMessage.h"



#include "MsgHeader.h"

/*
����������ԭ��
1.�Ժ���Ը����ڴ�ط���
2.�첽���õ�ʱ���ʺϷ������
*/




class CustomMessage : public IMessage
{
public:
	CustomMessage(int msgType);

private:
	// ��Ϣͷ
	struct MsgHeader customMsgHeader;
	

public:
	bool DecoderMsgHeader();
};



#endif