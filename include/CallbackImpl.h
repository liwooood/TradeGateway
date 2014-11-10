#ifndef CALLBACK_IMPL
#define CALLBACK_IMPL

#include "stdafx.h"

#include <string>
#include <map>

#include "t2sdk_interface.h"

class CCallbackImpl : public CCallbackInterface
{
private:
	std::string SOH;
	

	// Ӧ���¼�
	HANDLE hResEvent;

	// �����ж��¼�
	HANDLE hCloseEvent;

	// ��־�ͽ�����趨��ı���
	std::string funcId; // ��Ҫ���Ͳ㴫��
	std::string request;
	std::string account;

	
	int status;
	std::string errMsg;
	std::string errCode;
	std::string response;

private:
	
	void RetNoRecordRes();
	void GenResponse(int nErrCode, std::string sErrMsg);
	void debugOutput();

public:
	CCallbackImpl();

	// ��ΪCCallbackInterface�����մ��������IKnown��������Ҫʵ��һ����3������
    unsigned long  FUNCTION_CALL_MODE QueryInterface(const char *iid, IKnown **ppv);
    unsigned long  FUNCTION_CALL_MODE AddRef();
    unsigned long  FUNCTION_CALL_MODE Release();

    // �����¼�����ʱ�Ļص�������ʵ��ʹ��ʱ���Ը�����Ҫ��ѡ��ʵ�֣����ڲ���Ҫ���¼��ص���������ֱ��return
    // Reserved?Ϊ����������Ϊ�Ժ���չ��׼����ʵ��ʱ��ֱ��return��return 0��
    void FUNCTION_CALL_MODE OnConnect(CConnectionInterface *lpConnection);
    void FUNCTION_CALL_MODE OnSafeConnect(CConnectionInterface *lpConnection);
    void FUNCTION_CALL_MODE OnRegister(CConnectionInterface *lpConnection);
    void FUNCTION_CALL_MODE OnClose(CConnectionInterface *lpConnection);
    void FUNCTION_CALL_MODE OnSent(CConnectionInterface *lpConnection, int hSend, void *reserved1, void *reserved2, int nQueuingData);
    void FUNCTION_CALL_MODE Reserved1(void *a, void *b, void *c, void *d);
    void FUNCTION_CALL_MODE Reserved2(void *a, void *b, void *c, void *d);
    int  FUNCTION_CALL_MODE Reserved3();
    void FUNCTION_CALL_MODE Reserved4();
    void FUNCTION_CALL_MODE Reserved5();
    void FUNCTION_CALL_MODE Reserved6();
    void FUNCTION_CALL_MODE Reserved7();
    void FUNCTION_CALL_MODE OnReceivedBiz(CConnectionInterface *lpConnection, int hSend, const void *lpUnPackerOrStr, int nResult);
	void FUNCTION_CALL_MODE OnReceivedBizEx(CConnectionInterface *lpConnection, int hSend, LPRET_DATA lpRetData, const void *lpUnpackerOrStr, int nResult);
	void FUNCTION_CALL_MODE OnReceivedBizMsg(CConnectionInterface *lpConnection, int hSend, IBizMessage* lpMsg);

	

	void SetCloseEvent(HANDLE hCloseEvent);
	void SetResponseEvent(HANDLE hResEvent);


	
	void SetRequest(std::string req, std::string fid, std::string acc);

	struct hundsun_t2_async_return getResponse();
	
};
#endif