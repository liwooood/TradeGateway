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
			// 不应该有结果集返回
			if (!it->second.hasResultRet)
			{
				gFileLog::instance().Log(funcId + "不应该返回结果集");

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
			// 设置当前结果集
			lpUnPacker->SetCurrentDatasetByIndex(i);

			for (int c = 0; c < nCols; c++)
			{
				std::string sColName = lpUnPacker->GetColName(c);
					
				response += sColName;
					
				response += SOH;
			} // end all column

			// 打印所有记录
			for (int r = 0; r < nRows; r++)
			{
				// 打印每条记录
				for (int c = 0; c < nCols; c++)
				{
					std::string temp = lpUnPacker->GetStrByIndex(c);

					// 国泰君安一户通特殊处理
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
		GenResponse(nResult, "业务包解包失败");
		
		goto FINISH;
	}
	else
	{
		
		GenResponse(nResult, lpConnection->GetErrorMsg(nResult));
	
		goto FINISH;
	}


FINISH:
	// 应答处理完成，触发事件，让业务层继续处理
	SetEvent(hResEvent);

}


// 生成请求成功，无数据返回信息
void CCallbackImpl::RetNoRecordRes()
{
	status = 1;
	errCode = "1";
	errMsg = "请求执行成功，柜台系统没有数据返回。";

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


// 生成错误信息
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
    

    // 在OnClose回调函数中激活事件，表示连接已经断开
	// ConnectT2::AutoConnect会重连
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

// 以下各回调方法的实现仅仅为演示使用
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
