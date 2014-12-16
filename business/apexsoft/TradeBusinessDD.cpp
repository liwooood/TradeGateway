#include "StdAfx.h"

#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string.hpp>


#include "TradeBusinessDD.h"
//#include "LogManager.h"
#include "ConnectManager.h"




//#include "aes.h"

#include "ConfigManager.h"



//#include "CacheData.h"

// ICU
//#include <unicode/putil.h>
//#include <unicode/ucnv.h>

#include "mybotan.h"
#include "FileLog.h"

#include "TradeGatewayGtja.h"


#include "fiddef.h"
#include "fixapi.h"
#include "DingDian.h"

#include "CacheData.h"


TradeBusinessDD::TradeBusinessDD()
{
	m_hHandle = -1;
}

TradeBusinessDD::~TradeBusinessDD(void)
{
}


bool TradeBusinessDD::Send(std::string& request, std::string& response, int& status, std::string& errCode, std::string& errMsg)
{
	bool bRet = true;

	std::map<std::string, long> mReturnField;
	int nRet = FALSE;
	std::string fbdm;
	std::string khh;
	long session = 0;

	std::string fid_code = "";
	std::string fid_message = "";
	std::string tmp;


	

	ParseRequest(request);

	// �Ͼ�֤ȯ��ֹ�޸����Ϲ���
	if ( (sysNo.compare("njzq_jlp") == 0 || sysNo.compare("njzq_flash") == 0) && funcid.compare("202002") == 0)
	{
		response = "1";
		response += SOH;
		response += "3";
		response += SOH;

		response += "cssweb_code";
		response += SOH;
		response += "cssweb_msg";
		response += SOH;
		response += "cssweb_gwInfo";
		response += SOH;

		response += "0";
		response += SOH;
		response += "�޸Ŀͻ������뵽��������!";
		response += SOH;
		response += "0.0.0.0:5000";
		response += SOH;

		status = 1;
		errCode = "";
		errMsg = "";
		//logLevel = Trade::TradeLog::INFO_LEVEL;

		goto FINISH;
	}
	/*
	if (funcid == "000000")
	{
		//1.�Խ����������b64����
		std::string newpwd = reqmap["newpwd"];

		char decoder[20];
		memset(decoder, 0x00, sizeof(decoder));
		int outlen;
		g_MyBotan.Base64Decoder(newpwd, decoder, &outlen);
		std::string unlock = decoder;
		
		//2.�Ե�¼������н��룬����
		std::string trdpwd = reqmap["trdpwd"];
		std::string login;
		bool bRet = g_MyBotan.Base16Decoder_AESDecrypt("AES-256/ECB/PKCS7", "29dlo*%AO+3i16BaweTw.lc!)61K{9^5", trdpwd, login);

		//3.�Ƚ���������
		if (unlock == login)
		{
			response = "1";
			response += SOH;
			response += "3";
			response += SOH;

			response += "cssweb_code";
			response += SOH;
			response += "cssweb_msg";
			response += SOH;
			response += "cssweb_gwInfo";
			response += SOH;

			response += "1";
			response += SOH;
			response += "�����ɹ�!";
			response += SOH;
			response += "0.0.0.0:5000";
			response += SOH;

			status = 1;
			errCode = "";
			errMsg = "";
			//logLevel = Trade::TradeLog::INFO_LEVEL;

			goto FINISH;
		}
		else
		{
			response = "1";
			response += SOH;
			response += "3";
			response += SOH;

			response += "cssweb_code";
			response += SOH;
			response += "cssweb_msg";
			response += SOH;
			response += "cssweb_gwInfo";
			response += SOH;

			response += "1100";
			response += SOH;
			response += "����ʧ��!";
			response += SOH;
			response += "0.0.0.0:5000";
			response += SOH;

			status = 0;
			errCode = "1100";
			errMsg = "����ʧ��";
			//logLevel = Trade::TradeLog::INFO_LEVEL;

			goto FINISH;
		}
	}
	*/

	
	if (!cssweb_cacheFlag.empty())
	{
		if (g_CacheData.m_mapCacheData.find(cssweb_cacheFlag) != g_CacheData.m_mapCacheData.end())
		{
			std::string value = g_CacheData.m_mapCacheData[cssweb_cacheFlag];
		
			if (!value.empty())
			{
				response = value;

				status = 1;
				errCode = "";
				errMsg = "";
				//logLevel = Trade::TradeLog::INFO_LEVEL;

				gFileLog::instance().Log("cache use " + cssweb_cacheFlag);
				goto FINISH;
			}
		}
	}
	

	// �жϹ��ܺ��Ƿ��Ѷ���
	if (g_DingDian.m_mReturnField.find(funcid) == g_DingDian.m_mReturnField.end())
	{
			response = "1";
			response += SOH;
			response += "3";
			response += SOH;

			response += "cssweb_code";
			response += SOH;
			response += "cssweb_msg";
			response += SOH;
			response += "cssweb_gwInfo";
			response += SOH;

			response += "1100";
			response += SOH;
			response += "���ܺ�û������!";
			response += SOH;
			response += "0.0.0.0:5000";
			response += SOH;

			status = 0;
			errCode = "2001";
			errMsg = "���ܺ�û������";
			//logLevel = Trade::TradeLog::ERROR_LEVEL;

			goto FINISH;
	}

	
	
	
	session = Fix_AllocateSession(m_hHandle);
	
	// set gydm
	//gFileLog::instance().Log("set gydm:" + m_pConn->m_Counter->m_sGydm);
	Fix_SetGYDM(session, m_Counter->m_sGydm.c_str());
	

	// set fbdm & dest fbdm
	
	
	khh = reqmap["FID_KHH"];
	if (!khh.empty())
	{
		 fbdm = khh.substr(0,4);
	}
	else
	{
		std::string zjzh = reqmap["FID_ZJZH"];
		fbdm = zjzh.substr(0,4);
	}
	//gFileLog::instance().Log("set fbdm:"+fbdm);
	Fix_SetFBDM(session, fbdm.c_str());
	Fix_SetDestFBDM(session, fbdm.c_str());



	// set wtfs
	
	if (sysVer.compare("iphone")==0 || sysVer.compare("aphone") == 0)
	{
		//gFileLog::instance().Log("set wtfs:"+m_pConn->m_Counter->m_sWtfs_mobile);
		/*
		Fix_SetWTFS(session, m_Counter->m_sWtfs_mobile.c_str());

		std::string wtfs;
		if (sysVer == "iphone")
			wtfs = "iphoneί�з�ʽ=" + m_Counter->m_sWtfs_mobile;
		if (sysVer == "android")
			wtfs = "aphoneί�з�ʽ=" + m_Counter->m_sWtfs_mobile;

		//gFileLog::instance().Log(wtfs);
		*/
	}
	else
	{
		
	//	Fix_SetWTFS(session, m_Counter->m_sWtfs_web.c_str());

//		std::string wtfs = "webί�з�ʽ=" + m_Counter->m_sWtfs_mobile;

		//gFileLog::instance().Log(wtfs);
	}



	// set node
	
	if (sysVer.compare("iphone")==0 || sysVer.compare("aphone")==0)
	{
		std::string node;
		node =reqmap["cssweb_note"];

		//ǰ�˸�ʽ18621112013
		if (node.find("ZRSJ") == std::string::npos)
			node = "ZRSJ:" + node;

	
		Fix_SetNode(session, node.c_str());
	}
	else
	{
		std::string hardinfo;
		hardinfo = reqmap["cssweb_hardinfo"];

		std::string ip;
		ip = reqmap["cssweb_note"];

		std::string mac = "";
		std::string diskid = "";

		GetMacAndDiskID(hardinfo, mac, diskid);

		//ZR:IP:MAC:DiskID
		std::string node = "ZR:" + ip + ":" + mac + ":" + diskid;

		Fix_SetNode(session, node.c_str());
	}


	//gFileLog::instance().Log("Fix_CreateHead" + funcid);
	Fix_CreateHead(session, boost::lexical_cast<long>(funcid));

	
	//gFileLog::instance().Log("==��ʼ�����������==");
	for (std::map<std::string, std::string>::iterator it = reqmap.begin(); it != reqmap.end(); it++)
	{
		std::string key = it->first;
		std::string value = it->second;

		//std::string tmp = "key=" + key;
		//tmp += ",value=" + value;
		//gFileLog::instance().Log(tmp);

		if (FilterRequestField(key))
		{
			continue;
		}
		else if(key.compare("client_id")==0)
		{
			continue;
		}
		else if(key.compare("branch_no")==0)
		{
			continue;
		}
		else if (key.compare("FBDM")==0)
		{
			Fix_SetFBDM(session, value.c_str());
			Fix_SetDestFBDM(session, value.c_str());
		}
		else if (key.compare("FID_JYMM")==0)
		{
			char szPwd[256];
				memset(szPwd, 0x00, sizeof(szPwd));

				EncryptPwd(value.c_str(), szPwd);
				if (szPwd != NULL)
				{
					long l = g_DingDian.m_mRequestField[key];
					Fix_SetItem(session, l, szPwd);
				}

				/*
			
			// ��¼
			if (funcid.compare("190101")==0)
			{
				if (sysVer.compare("web")==0 || sysVer.compare("flash")==0)
				{
					//����FID_JMLX=2�����в���Ҫ���ܴ�����̨
					std::string pwd;
	
					bool bRet = g_MyBotan.Base16Decoder_AESDecrypt("AES-256/ECB/PKCS7", "29dlo*%AO+3i16BaweTw.lc!)61K{9^5", value, pwd);

					long l = g_DingDian.m_mRequestField[key];
					Fix_SetItem(session, l, pwd.c_str());
				}
				else
				{
					// iphone, aphone
					//û��FID_JMLX=2��������Ҫ���ܴ�����̨

					char szPwd[256];
					memset(szPwd, 0x00, sizeof(szPwd));

					EncryptPwd(value.c_str(), szPwd);
					if (szPwd != NULL)
					{
						long l = g_DingDian.m_mRequestField[key];
						Fix_SetItem(session, l, szPwd);
					}
				}
			}
			else
			{
				char szPwd[256];
				memset(szPwd, 0x00, sizeof(szPwd));

				EncryptPwd(value.c_str(), szPwd);
				if (szPwd != NULL)
				{
					long l = g_DingDian.m_mRequestField[key];
					Fix_SetItem(session, l, szPwd);
				}
			}
			*/
		}
		else if(key.compare("FID_MM")==0 || key.compare("FID_NEWMM")==0 || key.compare("FID_ZJMM")==0 || key.compare("FID_WBZHMM")==0)
		{
			/*
			if (sysVer.compare("flash")==0)
			{
				
				if (cssweb_pwdType.compare("1")==0)
				{
					// ��ȫģʽ
					std::string pwd;
					bool bRet = g_MyBotan.Base16Decoder_AESDecrypt("AES-256/ECB/PKCS7", "29dlo*%AO+3i16BaweTw.lc!)61K{9^5", value, pwd);

					char szPwd[256];
					memset(szPwd, 0x00, sizeof(szPwd));

					EncryptPwd(pwd.c_str(), szPwd);
					if (szPwd != NULL)
					{
						long l = g_DingDian.m_mRequestField[key];
						Fix_SetItem(session, l, szPwd);
					}
				}
				else
				{
					// ��ͨģʽ
					char decoder[20];
					memset(decoder, 0x00, sizeof(decoder));
					int outlen;
					bool bRet = g_MyBotan.Base64Decoder(value, decoder, &outlen);

					std::string pwd = decoder;

					char szPwd[256];
					memset(szPwd, 0x00, sizeof(szPwd));

					EncryptPwd(pwd.c_str(), szPwd);
					if (szPwd != NULL)
					{
						long l = g_DingDian.m_mRequestField[key];
						Fix_SetItem(session, l, szPwd);
					}

					if (key.compare("FID_NEWMM")==0)
					{
						// ������
						bool bRet = g_MyBotan.AESEncrypt_Base16Encoder("AES-256/ECB/PKCS7", "29dlo*%AO+3i16BaweTw.lc!)61K{9^5", pwd, flash_normal_modifypwd_newpwd);
						gFileLog::instance().Log("������"+pwd+"������" + flash_normal_modifypwd_newpwd);
					}
				}
			}
			else if (sysVer.compare("web")==0)
			{
					// ��ȫģʽ
					std::string pwd;
					bool bRet = g_MyBotan.Base16Decoder_AESDecrypt("AES-256/ECB/PKCS7", "29dlo*%AO+3i16BaweTw.lc!)61K{9^5", value, pwd);

					char szPwd[256];
					memset(szPwd, 0x00, sizeof(szPwd));

					EncryptPwd(pwd.c_str(), szPwd);
					if (szPwd != NULL)
					{
						long l = g_DingDian.m_mRequestField[key];
						Fix_SetItem(session, l, szPwd);
					}
			}
			else
			{*/
				char szPwd[256];
				memset(szPwd, 0x00, sizeof(szPwd));

				EncryptPwd(value.c_str(), szPwd);
				if (szPwd != NULL)
				{
					long l = g_DingDian.m_mRequestField[key];
					Fix_SetItem(session, l, szPwd);
				}
			//}
		}
		else
		{
			//if (g_DingDian.m_mRequestField.find(key) == g_DingDian.m_mRequestField.end())
			//{
			//	RetErrRes(Trade::TradeLog::ERROR_LEVEL, response, "2000", "�ֶ�ֵ��tradeparam.map���޷��ҵ�");
			//	goto FINISH;
			//}
			//else
			//{
				long l = g_DingDian.m_mRequestField[key];
				//gFileLog::instance().Log("Fix_SetItem:" + key);
				//TRACE("%s=%s\n", key.c_str(),  value.c_str());
				Fix_SetItem(session, l, value.c_str());
			//}
		}
	} // end for
	//gFileLog::instance().Log("==���������������==");


	// Ĭ��Ϊ30��
	Fix_SetTimeOut(session, m_Counter->m_nRecvTimeout);

	nRet = Fix_Run(session);
	if (!nRet)
	{
		long lErrCode = Fix_GetCode(session);
		char szErrMsg[1024];
		Fix_GetErrMsg(session, szErrMsg, sizeof(szErrMsg));

		GenResponse(lErrCode, szErrMsg, response, status, errCode, errMsg);

		goto FINISH;
	}	



	char szValue[512];

	memset(szValue, '\0', sizeof(szValue));
	Fix_GetItem(session, FID_CODE, szValue, sizeof(szValue), 0);
	fid_code = szValue;
	

	memset(szValue, '\0', sizeof(szValue));
	Fix_GetItem(session, FID_MESSAGE, szValue, sizeof(szValue), 0);
	fid_message = szValue;
	
	//tmp = "���ܺţ�" + funcid + "fid_code: " + fid_code;
	//gFileLog::instance().Log(tmp);

	int fidcode = -1;
	if (!fid_code.empty())
	{
		fidcode = boost::lexical_cast<int>(fid_code);
	}

	if (fid_code.empty() || fidcode >= 0)
	{
		
		long nRows = Fix_GetCount(session);
		//gFileLog::instance().Log("Fix_GetCount");

		// ����ɹ��������ݷ���
		if (nRows == 0)
		{
			RetNoRecordRes(response, status);
			status = 1;
			goto FINISH;
		}

		//gFileLog::instance().Log("��ʼ��������");
		mReturnField = g_DingDian.m_mReturnField[funcid];
		int nCols = mReturnField.size();

		//flash���ף���ͨģʽ���޸����빦��
		//if (sysVer.compare("flash")==0 && cssweb_pwdType.compare("0")==0 && funcid.compare("202010")==0)
		////{
			//nCols += 1;
		//}

		response = boost::lexical_cast<std::string>(nRows);
		response += SOH;
		response += boost::lexical_cast<std::string>(nCols);
		response += SOH;

		


		// ��ͷ
		for (std::map<std::string, long>::iterator it = mReturnField.begin(); it != mReturnField.end(); it++)
		{
			std::string sColName = it->first;

			response += sColName;
			response += SOH;
		}

		//flash���ף���ͨģʽ���޸����빦��
		//if (sysVer.compare("flash")==0 && cssweb_pwdType.compare("0")==0 && funcid.compare("202010")==0)
		//{
		//	response += "cssweb_pwd";
		//	response += SOH;
		//}


		for (int row=0; row<nRows; row++)
		{
			char szValue[512];
			for (std::map<std::string, long>::iterator it = mReturnField.begin(); it != mReturnField.end(); it++)
			{
				std::string sKey = it->first;
				long lKey = it->second;

			
				memset(szValue, '\0', sizeof(szValue));
				Fix_GetItem(session, lKey, szValue, sizeof(szValue), row);

				response += szValue;
				response += SOH;
			} // end for col
		} // end for row

		//flash���ף���ͨģʽ���޸����빦��
		//if (sysVer=="flash" && cssweb_pwdType == "0" && funcid=="202010")
		//{
		//	response += flash_normal_modifypwd_newpwd;
		//	response += SOH;
		//}

		status = 1;
		errCode = "";
		errMsg = "";
		//logLevel = Trade::TradeLog::INFO_LEVEL;

		// ���������
		if (!cssweb_cacheFlag.empty())
		{
			gFileLog::instance().Log("���뻺��" + cssweb_cacheFlag);
			g_CacheData.m_mapCacheData[cssweb_cacheFlag] = response;
			
		}
	}
	else
	{
		GenResponse(fidcode, fid_message, response, status, errCode, errMsg);
		goto FINISH;
	}






FINISH:
	
	if (session != 0)
	{
		Fix_ReleaseSession(session);
	}

	return bRet;

}


char* TradeBusinessDD::EncryptPwd(const char* plainText, char* szPwd)
{

	if (plainText == NULL)
		return NULL;

	strcpy(szPwd, plainText);
	Fix_Encode(szPwd);
	return szPwd;
}


bool TradeBusinessDD::CreateConnect()
{
	

	int nRet = 0;

	

		std::string gtAddr = m_Counter->m_sIP + "@" + boost::lexical_cast<std::string>(m_Counter->m_nPort) + "/tcp";
		m_hHandle = Fix_Connect(gtAddr.c_str(), m_Counter->m_sUserName.c_str(), m_Counter->m_sPassword.c_str(), m_Counter->m_nConnectTimeout); 

		
		if (m_hHandle == 0)
		{
			return false;
		}
		else
		{
			return true;
		}

}

void TradeBusinessDD::CloseConnect()
{
	if (m_hHandle != -1)
	{
		Fix_Close(m_hHandle);
		m_hHandle = -1;
	}
}

void TradeBusinessDD::GetMacAndDiskID(std::string hardinfo, std::string& mac, std::string& diskid)
{
	std::vector<std::string> info;
	boost::split(info, hardinfo, boost::is_any_of("|")); // ע����Ҫͨ�������ļ�����
	int size = info.size();
	
	if (size < 2)
		return;

	mac = info[0];

	diskid = info[1];
}
