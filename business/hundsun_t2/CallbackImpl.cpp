#include "callbackimpl.h"

unsigned long CCallbackImpl::QueryInterface(const char *iid, IKnown **ppv)
{
    return 0;
}

unsigned long CCallbackImpl::AddRef()
{
    return 0;
}

unsigned long CCallbackImpl::Release()
{
    return 0;
}

// ���¸��ص�������ʵ�ֽ���Ϊ��ʾʹ��
void CCallbackImpl::OnConnect(CConnectionInterface *lpConnection)
{
  
}

void CCallbackImpl::OnSafeConnect(CConnectionInterface *lpConnection)
{
   
}

void CCallbackImpl::OnRegister(CConnectionInterface *lpConnection)
{
    
}



void CCallbackImpl::OnSent(CConnectionInterface *lpConnection, int hSend, void *reserved1, void *reserved2, int nQueuingData)
{
}

void CCallbackImpl::Reserved1(void *a, void *b, void *c, void *d)
{
}

void CCallbackImpl::Reserved2(void *a, void *b, void *c, void *d)
{
}

void CCallbackImpl::OnReceivedBizEx(CConnectionInterface *lpConnection, int hSend, LPRET_DATA lpRetData, const void *lpUnpackerOrStr, int nResult)
{
	
}

void CCallbackImpl::OnClose(CConnectionInterface *lpConnection)
{
    

    // ��OnClose�ص������м����¼�����ʾ�����Ѿ��Ͽ�
	// ConnectT2::AutoConnect������
    SetEvent(hCloseEvent);
}

void CCallbackImpl::OnReceivedBiz(CConnectionInterface *lpConnection, int hSend, const void *lpUnPackerOrStr, int nResult)
{
	SetEvent(hResEvent);

	/*
    switch (nResult)
    {
    case 0:
        {
            puts("ҵ������ɹ���");
            ShowPacket((IF2UnPacker *)lpUnPackerOrStr);
            break;
        }

    case 1:
        {
            puts("ҵ�����ʧ�ܡ�");
            ShowPacket((IF2UnPacker *)lpUnPackerOrStr);
            break;
        }

    case 2:
        {
            puts((char *)lpUnPackerOrStr);
            break;
        }

    default:
        {
            puts("ҵ������ʧ�ܡ�");
            break;
        }
    }
	*/
}

std::string CCallbackImpl::getResponse()
{
	return response;
}

int CCallbackImpl::Reserved3()
{
    return 0;
}

void CCallbackImpl::Reserved4()
{
}

void CCallbackImpl::Reserved5()
{
}

void CCallbackImpl::Reserved6()
{
}

void CCallbackImpl::Reserved7()
{
}

void CCallbackImpl::OnReceivedBizMsg(CConnectionInterface *lpConnection, int hSend, IBizMessage* lpMsg)
{
}

void CCallbackImpl::SetCloseEvent(HANDLE& closeEvent)
{
	this->hCloseEvent = closeEvent;
}
