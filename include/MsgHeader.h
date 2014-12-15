#ifndef CUSTOM_MESSAGE_HEADER_H
#define CUSTOM_MESSAGE_HEADER_H
/*
ֱ�Ӳ��ö�����Э��
����ͽ���ͳһ
������IMͳһ
����tcp��sslͳһ
ȫ�����ڸ�Ϊ���ڻỰģʽͨ��
Ҫ֧�ּ���ģʽ��?
*/

//���Ҫ֧�ֳ�ssl��ĻỰ����ԿЭ�飬ÿ��socket����һ��aes��Կ

// ������δ���

#define MSG_TYPE_REQUEST 0 // ����
#define MSG_TYPE_RESPONSE_ERROR 1 // Ӧ�����
#define MSG_TYPE_RESPONSE_MORE 2 // Ӧ�����
#define MSG_TYPE_RESPONSE_END 3 // Ӧ�����
#define MSG_TYPE_RESPONSE_PUSH 4 // ����������

#define FUNCTION_HEARTBEAT 0 // ����
#define FUNCTION_CAPTCHA 1 // ��֤��


/*
���� ����/Ӧ��
     ����/����

���� ����/Ӧ��

*/

// 1�ֽڶ���
#pragma pack(1)
typedef struct MsgHeader
{
	int MsgContentSize; // ��Ϣ���ݴ�С
	int CRC; // ����
	unsigned char zip; // �Ƿ�֧��ѹ��
	unsigned char MsgType; //��Ϣ���� 
	int FunctionNo; // ���ܺ�

	//unsigned char SystemId; // �ͻ���ϵͳ���
	//unsigned char SystemVersion; // �ͻ����ն�����
	//unsigned char BusinessType; // ҵ������

	
}MSG_HEADER, *PMSG_HEADER;

#pragma pack()

// ����Ӧ��
/*
int nErrCode;
char szErrMsg; [pkgbodysize - sizeof(int)]
*/

#endif
