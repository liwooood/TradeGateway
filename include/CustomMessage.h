#ifndef CUSTOM_MESSAGE_H
#define CUSTOM_MESSAGE_H

#include <vector>

#include <boost/array.hpp>
#include <boost/scoped_array.hpp> 
#include <boost/noncopyable.hpp>
#include <boost/checked_delete.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include "MsgHeader.h"


#include "IMessage.h"

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
	struct MsgHeader msgHeader;
	

public:
	bool DecoderMsgHeader();


	/*
	PMSG_HEADER GetMsgHeader();
	


	
	char * GetMsgContent();
	
	

	
	

	
	

	*/

	
};



#endif