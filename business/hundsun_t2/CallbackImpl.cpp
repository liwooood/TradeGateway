#include "callbackimpl.h"

#include <boost/lexical_cast.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string.hpp>

#include "FileLogManager.h"
#include "FileLog.h"


CCallbackImpl::CCallbackImpl()
{
	SOH = "\x01";
}

void CCallbackImpl::SetFuncId(std::string funcid)
{
	funcId = funcid;
}

void CCallbackImpl::OnReceivedBiz(CConnectionInterface *lpConnection, int hSend, const void *Pointer, int nResult)
{
	status = 0;
	errCode = "";
	errMsg = "";
	response = "";

	if (nResult == 0)
	{
		IF2UnPacker *lpUnPacker = (IF2UnPacker *)Pointer;

		

		int nRows = lpUnPacker->GetRowCount();
		int nCols = lpUnPacker->GetColCount();

		if (nRows == 0)
		{
			RetNoRecordRes();
			goto FINISH;
		}
		
		
		std::map<std::string, FUNCTION_DESC>::iterator it = gFileLogManager::instance().m_mT2_FilterFunc.find(funcId);
		if (it != gFileLogManager::instance().m_mT2_FilterFunc.end())
		{
			// ��Ӧ���н��������
			if (!it->second.hasResultRet)
			{
				gFileLog::instance().Log(funcId + "��Ӧ�÷��ؽ����");

				RetNoRecordRes();
				
				
				goto FINISH;
			}
		}
		

		response = boost::lexical_cast<std::string>(nRows);
		response += SOH;
		response += boost::lexical_cast<std::string>(nCols);
		response += SOH;
	
		for (int i = 0; i < lpUnPacker->GetDatasetCount(); ++i)
		{
			// ���õ�ǰ�����
			lpUnPacker->SetCurrentDatasetByIndex(i);

			for (int c = 0; c < nCols; c++)
			{
				std::string sColName = lpUnPacker->GetColName(c);
					
				response += sColName;
					
				response += SOH;
			} // end all column

			// ��ӡ���м�¼
			for (int r = 0; r < nRows; r++)
			{
				// ��ӡÿ����¼
				for (int c = 0; c < nCols; c++)
				{
					std::string temp = lpUnPacker->GetStrByIndex(c);

					// ��̩����һ��ͨ���⴦��
					if (funcId == "337014")
						boost::algorithm::replace_all(temp, SOH, "");
					
					response += temp;
					
					response += SOH;
				} // end all column

				lpUnPacker->Next();
			} // end for all row

		} // end for GetDatasetCount()

		status = 1;
		errCode = "";
		errMsg = "";
		
		//logLevel = Trade::TradeLog::INFO_LEVEL;

	}
	else if(nResult == 1)
	{
		IF2UnPacker *lpUnPacker = (IF2UnPacker *)Pointer;
		errMsg = lpUnPacker->GetStr("error_info");
		GenResponse(boost::lexical_cast<int>(lpUnPacker->GetStr("error_no")), errMsg);
		
		
		goto FINISH;
	}
	else if(nResult == 2)
	{
		errMsg = (char*)Pointer;
		
		GenResponse(nResult, errMsg);
		
		goto FINISH;
	}
	else if (nResult == 3)
	{
		GenResponse(nResult, "ҵ������ʧ��");
		
		goto FINISH;
	}
	else
	{
		
		GenResponse(nResult, lpConnection->GetErrorMsg(nResult));
	
		goto FINISH;
	}


FINISH:
	// Ӧ������ɣ������¼�����ҵ����������
	SetEvent(hResEvent);

}


// ��������ɹ��������ݷ�����Ϣ
void CCallbackImpl::RetNoRecordRes()
{
	status = 1;
	errCode = "1";
	errMsg = "����ִ�гɹ�����̨ϵͳû�����ݷ��ء�";

	response = "1";
	response += SOH;
	response += "2";
	response += SOH;

	response += "cssweb_code";
	response += SOH;
	response += "cssweb_msg";
	response += SOH;

	response += errCode;
	response += SOH;
	response += errMsg;
	response += SOH;

}


// ���ɴ�����Ϣ
void CCallbackImpl::GenResponse(int nErrCode, std::string sErrMsg)
{
	status = 0;

	errCode = boost::lexical_cast<std::string>(nErrCode);
	errMsg = sErrMsg;

	response = "1";
	response += SOH;
	response += "2";
	response += SOH;

	response += "cssweb_code";
	response += SOH;
	response += "cssweb_msg";
	response += SOH;

	response += errCode;
	response += SOH;
	response += errMsg;
	response += SOH;
}


void CCallbackImpl::OnClose(CConnectionInterface *lpConnection)
{
    

    // ��OnClose�ص������м����¼�����ʾ�����Ѿ��Ͽ�
	// ConnectT2::AutoConnect������
    SetEvent(hCloseEvent);
}


void CCallbackImpl::SetCloseEvent(HANDLE closeEvent)
{
	this->hCloseEvent = closeEvent;
}

void CCallbackImpl::SetResponseEvent(HANDLE responseEvent)
{
	this->hResEvent = responseEvent;
}

std::string CCallbackImpl::getResponse()
{
	return response;
}

int CCallbackImpl::getStatus()
{
	return status;
}

std::string CCallbackImpl::getErrCode()
{
	return errCode;
}

std::string CCallbackImpl::getErrMsg()
{
	return errMsg;
}


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
