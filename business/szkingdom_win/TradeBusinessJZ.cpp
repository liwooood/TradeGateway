#include "StdAfx.h"


#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string.hpp>

#include "KDEncodeCli.h"
#include "kcbpcli.h"

#include "TradeBusinessJZ.h"
//#include "LogManager.h"
#include "ConnectManager.h"
#include "ConfigManager.h"
#include "Dogskin.h"
//ICU
//#include <unicode/putil.h>
//#include <unicode/ucnv.h>
#include "mybotan.h"

#include "FileLog.h"


TradeBusinessJZ::TradeBusinessJZ()
{
	handle = NULL;
}

TradeBusinessJZ::TradeBusinessJZ(int connId, Counter counter)
{
	this->connId = connId;
	this->counter = counter;
}

TradeBusinessJZ::~TradeBusinessJZ(void)
{
}


bool TradeBusinessJZ::CreateConnect()
{
	int nRet = 0;


		tagKCBPConnectOption stKCBPConnection;
		memset(&stKCBPConnection, 0x00, sizeof(stKCBPConnection));

		strcpy(stKCBPConnection.szServerName, counter.m_sServerName.c_str());
		stKCBPConnection.nProtocal = 0;
		strcpy(stKCBPConnection.szAddress, counter.m_sIP.c_str());
		stKCBPConnection.nPort = counter.m_nPort;
		strcpy(stKCBPConnection.szSendQName, counter.m_sReq.c_str());
		strcpy(stKCBPConnection.szReceiveQName, counter.m_sRes.c_str());

		nRet = KCBPCLI_Init( &handle );
		nRet = KCBPCLI_SetConnectOption( handle, stKCBPConnection );		

		//���ó�ʱ
		nRet = KCBPCLI_SetCliTimeOut( handle, counter.m_nConnectTimeout);

		// �����Ƿ����������Ϣ
		//nRet = KCBPCLI_SetOptions( handle, KCBP_OPTION_TRACE, &gConfigManager::instance().m_nIsTradeServerDebug, sizeof(int));

		nRet = KCBPCLI_SQLConnect( handle, stKCBPConnection.szServerName, (char*)counter.m_sUserName.c_str(), (char*)counter.m_sPassword.c_str());

		if (nRet != 0)
		{
			

			return false;
		}
		else
		{
			

			return true;
		}
	
}

void TradeBusinessJZ::CloseConnect()
{
	if (handle != NULL)
	{
		KCBPCLI_SQLDisconnect(handle);
		KCBPCLI_Exit(handle);
		handle = NULL;
	}
}

bool TradeBusinessJZ::Send(std::string& request, std::string& response, int& status, std::string& errCode, std::string& errMsg)
{
	bool bRet = true;
	int nRet = 0;
	
	std::string pkgType = "2";
	std::string log;
	std::string content = "";
	int nRows = 0;
	char szErrMsg[512] = {0};

	ParseRequest(request);


	

	nRet = KCBPCLI_BeginWrite(handle);

	KCBPCLI_SetSystemParam(handle, KCBP_PARAM_SERVICENAME, (char*)funcid.c_str());
	KCBPCLI_SetSystemParam(handle, KCBP_PARAM_RESERVED, (char*)reqmap["orgid"].c_str());
	KCBPCLI_SetSystemParam(handle, KCBP_PARAM_PACKETTYPE, (char*)pkgType.c_str());

	for (std::map<std::string, std::string>::iterator it = reqmap.begin(); it != reqmap.end(); it++)
	{
		std::string key = it->first;
		std::string value = it->second;
		 

		if (FilterRequestField(key))
		{
			continue;
		}		
		else if(key == "trdpwd")
		{
				std::string encrypt_pwd = g_Encrypt.EncryptPWD(value);
				KCBPCLI_SetValue(handle, (char*)key.c_str(), (char*)encrypt_pwd.c_str());

				OutputDebugString("�͵���̨�Ľ�������");
				OutputDebugString(encrypt_pwd.c_str());
				OutputDebugString("\n");
/*
			if (type == "flash")
			{
				if (funcid == "410302")
				{
					//�޸�����
					trdpwd = GetOtherPWD(isSafe, value);

					std::string encrypt_pwd = g_Encrypt.EncryptPWD(trdpwd);
					
					KCBPCLI_SetValue(m_pConn->handle, (char*)key.c_str(), (char*)encrypt_pwd.c_str());
				}
				else
				{
					// ��Ҫ�Ƚ���
					trdpwd = GetTradePWD(isSafe, value);

					std::string encrypt_pwd = g_Encrypt.EncryptPWD(trdpwd);
					
					KCBPCLI_SetValue(m_pConn->handle, (char*)key.c_str(), (char*)encrypt_pwd.c_str());


					OutputDebugString("�͵���̨�Ľ�������");
					OutputDebugString(encrypt_pwd.c_str());
					OutputDebugString("\n");
				}
			}
			else
			{

			}

			continue;
*/
/*
			char szEncryptPwd[33];
			memset(szEncryptPwd, 0, sizeof(szEncryptPwd));

			std::string enc_key = reqmap["custid"];


			if (type == "web")
			{
				if (funcid == "410301")
				{
					KDEncode(KDCOMPLEX_ENCODE, (unsigned char*)value.c_str(), value.length(), (unsigned char *)szEncryptPwd, sizeof(szEncryptPwd)-1, "410301", 6);
				}
				else
				{
					KDEncode(KDCOMPLEX_ENCODE, (unsigned char*)value.c_str(), value.length(), (unsigned char *)szEncryptPwd, sizeof(szEncryptPwd)-1, (void*)enc_key.c_str(), enc_key.length());
				}
			}
			else
			{
				std::string tradepwd_plain = GetTradePWD(isSafe, value);

				if (funcid == "410301")
				{
					KDEncode(KDCOMPLEX_ENCODE, (unsigned char*)tradepwd_plain.c_str(), tradepwd_plain.length(), (unsigned char *)szEncryptPwd, sizeof(szEncryptPwd)-1, "410301", 6);
				}
				else
				{
					KDEncode(KDCOMPLEX_ENCODE, (unsigned char*)tradepwd_plain.c_str(), tradepwd_plain.length(), (unsigned char *)szEncryptPwd, sizeof(szEncryptPwd)-1, (void*)enc_key.c_str(), enc_key.length());
				}
			}
			std::string encrypt_pwd = g_Encrypt.EncryptPWD("456456");
			//KCBPCLI_SetValue(m_pConn->handle, (char*)key.c_str(), szEncryptPwd);
			
			//KCBPCLI_SetValue(m_pConn->handle, (char*)key.c_str(), "");
			TRACE("��������...........................%s\n", szEncryptPwd);

			//7Ketq+ncv0rD+iX0Sgq8/A==
*/
		}
		else if (key == "newpwd")
		{
				std::string encrypt_pwd = g_Encrypt.EncryptPWD(value);

				KCBPCLI_SetValue(handle, (char*)key.c_str(), (char*)encrypt_pwd.c_str());	
/*
			if (type == "flash")
			{
				//����ǰ��
				newpwd = GetOtherPWD(isSafe, value);

				//�ù�̨����
				std::string encrypt_pwd = g_Encrypt.EncryptPWD(newpwd);

				KCBPCLI_SetValue(m_pConn->handle, (char*)key.c_str(), (char*)encrypt_pwd.c_str());

				// ��׼��¼���޸�����ɹ�����Ҫ���ܷ��ظ�ǰ�ˣ��´δ�����ܺ��������
				// ��ǿ��¼, �޸�����ɹ���ǰ���Լ�����������
				std::string aeskey = "23dpasd23d-23l;df9302hzv/3lvjh*5";

				char cipher[50];
				memset(cipher, 0x00, sizeof(cipher));
				int outlen;
				bool bRet = g_MyBotan.AESEncrypt("AES-256/ECB/PKCS7", aeskey, newpwd, cipher, &outlen);
				//char * out = aes_encrypt_encode(aeskey.c_str(), newpwd.c_str());
				if (bRet)
				{
					newpwd_enc = cipher;
					boost::algorithm::replace_all(newpwd_enc, "=", "$");
					//free(out);
				}
			}
			else
			{
				newpwd = value;
					
				//�ù�̨����
				std::string encrypt_pwd = g_Encrypt.EncryptPWD(newpwd);

				KCBPCLI_SetValue(m_pConn->handle, (char*)key.c_str(), (char*)encrypt_pwd.c_str());		

			}
			continue;
*/
/*
			char szEncryptPwd[33];
			memset(szEncryptPwd, 0, sizeof(szEncryptPwd));

			std::string enc_key = reqmap["custid"];


			if (type == "web")
			{
				
				KDEncode(KDCOMPLEX_ENCODE, (unsigned char*)value.c_str(), value.length(), (unsigned char *)szEncryptPwd, sizeof(szEncryptPwd)-1, (void*)enc_key.c_str(), enc_key.length());
			}
			else
			{
				std::string tradepwd_plain = GetTradePWD(isSafe, value);

				KDEncode(KDCOMPLEX_ENCODE, (unsigned char*)tradepwd_plain.c_str(), tradepwd_plain.length(), (unsigned char *)szEncryptPwd, sizeof(szEncryptPwd)-1, (void*)enc_key.c_str(), enc_key.length());
			}

			KCBPCLI_SetValue(m_pConn->handle, (char*)key.c_str(), szEncryptPwd);

			continue;	
*/
		}

		else if(key == "fundpwd" || key == "newfundpwd" || key == "oldfundpwd")
		{
			std::string encrypt_pwd = g_Encrypt.EncryptPWD(value);
			KCBPCLI_SetValue(handle, (char*)key.c_str(), (char*)encrypt_pwd.c_str());		

			/*
			if (type == "flash")
			{
				//����ǰ��
				std::string sPwdPlain = GetOtherPWD(isSafe, value);

				//�ù�̨����
				std::string encrypt_pwd = g_Encrypt.EncryptPWD(sPwdPlain);

				KCBPCLI_SetValue(m_pConn->handle, (char*)key.c_str(), (char*)encrypt_pwd.c_str());			
			}
			else
			{
				//�ù�̨����
			}

			continue;
			*/

/*
				int KDEncode(int nEncode_Level, 
			  unsigned char *pSrcData, int nSrcDataLen, 
			  unsigned char *pDestData, int nDestDataBufLen, 
			  void *pKey, int nKeyLen);


			char szEncryptPwd[33];
			memset(szEncryptPwd, 0, sizeof(szEncryptPwd));

			std::string enc_key = reqmap["fundid"];


			if (type == "web")
			{
				
				KDEncode(KDCOMPLEX_ENCODE, (unsigned char*)value.c_str(), value.length(), (unsigned char *)szEncryptPwd, sizeof(szEncryptPwd)-1, (void*)enc_key.c_str(), enc_key.length());
			}
			else
			{
				std::string tradepwd_plain = GetTradePWD(isSafe, value);

				KDEncode(KDCOMPLEX_ENCODE, (unsigned char*)tradepwd_plain.c_str(), tradepwd_plain.length(), (unsigned char *)szEncryptPwd, sizeof(szEncryptPwd)-1, (void*)enc_key.c_str(), enc_key.length());
			}
*/
		}
//		else if(key == "ddasigndata")
//		{
//			if (type == "flash")
//			{
				//flash����ר��
//				boost::algorithm::replace_all(value, "$", "=");
//				KCBPCLI_SetValue(m_pConn->handle, (char*)key.c_str(), (char*)value.c_str());
//			}

//			continue;
//		}
		else
		{
			KCBPCLI_SetValue(handle, (char*)key.c_str(), (char*)value.c_str());
		}
	} // end for



	nRet  = KCBPCLI_SQLExecute(handle, (char*)funcid.c_str()); // ���ܺ�
	if (nRet != 0)
	{
		//gFileLog::instance().Log(funcid + " KCBPCLI_SQLExecute");

		//if (nRet == 2003 || nRet == 2004 || nRet == 2055 || nRet == 2054)
		//����
		//int nErrCode = 0;
		//char szErrMsg[2048] = {0};
		//KCBPCLI_GetErr(m_pConn->handle, &nErrCode, szErrMsg);

		//if (nErrCode == 0)
		//{
		//	RetNoRecordRes(response);
		//	goto FINISH;
		//}

		this->GenResponse(nRet, "KCBPCLI_SQLExecute error", response, status, errCode, errMsg);
		
		goto FINISH;
	}


	int nErrCode = 0;
	nRet = KCBPCLI_GetErrorCode(handle, &nErrCode);
	if (nErrCode != 0)
	{
		KCBPCLI_GetErrorMsg(handle, szErrMsg );
		
		this->GenResponse(nErrCode, szErrMsg, response, status, errCode, errMsg);
		goto FINISH;
	}


	nRet = KCBPCLI_RsOpen(handle);
	if (nRet != 0 && nRet != 100)
	{
		
		this->GenResponse(nRet, "KCBPCLI_RsOpen error", response, status, errCode, errMsg);
		goto FINISH;
	}


	nRet = KCBPCLI_SQLFetch(handle);

	char szTmpbuf[1024];
	memset(szTmpbuf, 0x00, sizeof(szTmpbuf));
	nRet = KCBPCLI_RsGetColByName(handle, "CODE", szTmpbuf );
	if (nRet != 0)
	{
		
		this->GenResponse(nRet, "KCBPCLI_RsGetColByName error", response, status, errCode, errMsg);
		goto FINISH;
	}

	if (strcmp(szTmpbuf, "0") != 0)
	{
		errCode = szTmpbuf;

		KCBPCLI_RsGetColByName(handle, "MSG", szTmpbuf );
		errMsg = szTmpbuf;

		
		this->GenResponse(boost::lexical_cast<int>(errCode), errMsg, response, status, errCode, errMsg);
		goto FINISH;
	}

	
	nRet = KCBPCLI_SQLMoreResults(handle);
	if (nRet != 0)
	{
		int nErrCode = 0;
		KCBPCLI_GetErrorCode(handle, &nErrCode);
		KCBPCLI_GetErrorMsg(handle, szErrMsg );

		if (nErrCode == 0)
		{
			RetNoRecordRes(response, status);
			goto FINISH;
		}


		
		this->GenResponse(nErrCode, szErrMsg, response, status, errCode, errMsg);
		goto FINISH;

		// �ǲ���ִ�гɹ���û�����ݷ���
	}
	

/*
	if (nRet != 0 )
	{
		RetErrRes(Cssweb::CsswebMessage::ERROR_LEVEL, response, response_pb, boost::lexical_cast<std::string>(nRet), "KCBPCLI_SQLMoreResults error");
		return;
	}
*/



	//nRet = KCBPCLI_SQLNumResultCols(m_pConn->handle, &nCol);
	

	
	nRows = 0;

	int nCols = 0;
	KCBPCLI_RsGetColNum(handle, &nCols);


	bool bGetColName = true;
	
				
	while( !KCBPCLI_RsFetchRow(handle) )
	{
		nRows += 1;

		// ȡ��ͷ
		if (bGetColName)
		{
			for( int i = 1; i <= nCols; i++ )
			{
				char szColName[256];
				memset(szColName, 0x00, sizeof(szColName));

				KCBPCLI_RsGetColName( handle, i, szColName, sizeof(szColName)-1 );

				content += szColName;
				content += SOH;
			}

			bGetColName = false;
		} // end for col

		//����������
		for( int i = 1; i <= nCols; i++ )
		{
			//��ȡ����
			char szColName[256];
			memset(szColName, 0x00, sizeof(szColName));
			KCBPCLI_RsGetColName( handle, i, szColName, sizeof(szColName)-1 );

			//����������ȡ��ֵ
			char szColValue[1024];
			KCBPCLI_RsGetColByName( handle, szColName, szColValue );

			content += szColValue;
			content += SOH;
		} // end for row
	} // end while

	log = funcid;
	log += "��������";
	log += boost::lexical_cast<std::string>(nRows);
	gFileLog::instance().Log(log);

	if (nRows == 0)
	{
		// ִ�гɹ�����̨û�����ݷ���
		RetNoRecordRes(response, status);
		goto FINISH;
	}

	response = boost::lexical_cast<std::string>(nRows);
	response += SOH;
	response += boost::lexical_cast<std::string>(nCols);
	response += SOH;

	response += content;


	nRet = KCBPCLI_SQLCloseCursor(handle);


	status = 1;
	errCode = "";
	errMsg = "";
	//logLevel = Trade::TradeLog::INFO_LEVEL;
	

FINISH:
	

	return bRet;
}
