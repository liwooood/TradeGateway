#include "stdafx.h"

#include "FileLogManager.h"

#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

// xml
#include "xml/rapidxml.hpp"
#include "xml/rapidxml_utils.hpp"

#include "ConfigManager.h"
#include "FileLog.h"




FileLogManager::FileLogManager(void)
	:file_worker_(file_q_, boost::bind(&FileLogManager::file_log, this, _1), gConfigManager::instance().m_nLogFileThreadPool)
{
}


FileLogManager::~FileLogManager(void)
{
}

void FileLogManager::start()
{	
	file_worker_.start();
}

void FileLogManager::stop()
{
	file_worker_.stop();
}


void FileLogManager::push(Trade::TradeLog logMsg)
{
	if (gConfigManager::instance().m_nLogFileEnable)
		file_q_.push(logMsg);	
}


// �ļ���־�̳߳ش�����
bool FileLogManager::file_log(Trade::TradeLog log)
{
	std::string SOH = "\x01";

	

	std::string funcid = log.funcid();

	// ��������¼
	if (funcid.empty() || funcid.compare("999999")==0)
	{
		return true;
	}

	std::string countertype = log.countertype();
	int counterType = COUNTER_TYPE_UNKNOWN;
	if (!countertype.empty())
	{
		counterType = boost::lexical_cast<int>(countertype);
	}

	// ����t2
	if (counterType == COUNTER_TYPE_HS_T2)
	{
		// ��������˹��˲�ѯ���ܺ�
		if (gConfigManager::instance().m_nFilterFuncId)
		{
			std::map<std::string, FUNCTION_DESC>::iterator it = m_mT2_FilterFunc.find(funcid);
			if (it != m_mT2_FilterFunc.end())
			{
				// ���˲�ѯ���ܺ�
				if (it->second.isQuery)
				{
					//log.destroy();
					
					return true;
				}
			}
		}
	}
	
	// ��֤
	if (counterType == COUNTER_TYPE_JZ_WIN)
	{
		// ��������˹��˲�ѯ���ܺ�
		if (gConfigManager::instance().m_nFilterFuncId)
		{
			std::map<std::string, FUNCTION_DESC>::iterator it = m_mKingdom_FilterFunc.find(funcid);
			if (it != m_mKingdom_FilterFunc.end())
			{
				// ���˲�ѯ���ܺ�
				if (it->second.isQuery)
				{
					//log.destroy();
					
					return true;
				}
			}
		}
	}

	// ����
	if (counterType == COUNTER_TYPE_DINGDIAN)
	{
		// ��������˹��˲�ѯ���ܺ�
		if (gConfigManager::instance().m_nFilterFuncId)
		{
			std::map<std::string, FUNCTION_DESC>::iterator it = m_mDingDian_FilterFunc.find(funcid);
			if (it != m_mDingDian_FilterFunc.end())
			{
				// ���˲�ѯ���ܺ�
				if (it->second.isQuery)
				{
					//log.destroy();
					
					return true;
				}
			}
		}
	}

	// ���˴�
	if (counterType == COUNTER_TYPE_JSD)
	{
		// ��������˹��˲�ѯ���ܺ�
		if (gConfigManager::instance().m_nFilterFuncId)
		{
			std::map<std::string, FUNCTION_DESC>::iterator it = m_mAGC_FilterFunc.find(funcid);
			if (it != m_mAGC_FilterFunc.end())
			{
				// ���˲�ѯ���ܺ�
				if (it->second.isQuery)
				{
					//log.destroy();
					
					return true;
				}
			}
		}
	}

	// �����̨
	if (counterType == COUNTER_TYPE_TEST || counterType == COUNTER_TYPE_XINYI)
	{
		// ��������˹��˲�ѯ���ܺ�
		if (gConfigManager::instance().m_nFilterFuncId)
		{
			std::map<std::string, FUNCTION_DESC>::iterator it = m_mTest_FilterFunc.find(funcid);
			if (it != m_mTest_FilterFunc.end())
			{
				// ���˲�ѯ���ܺ�
				if (it->second.isQuery)
				{
					//log.destroy();
					
					return true;
				}
			}
		}
	}

	std::string request = log.request();
	std::map<std::string, std::string> reqmap; // ��Ź��˵�����

	// �����ֶ�
	if (counterType == COUNTER_TYPE_HS_T2)
	{
		GetFilterMap(request, m_mT2_FilterField, reqmap);
	}
	else if (counterType == COUNTER_TYPE_JZ_WIN)
	{
		GetFilterMap(request, m_mKingdom_FilterField, reqmap);
	}
	else if (counterType == COUNTER_TYPE_DINGDIAN)
	{
		GetFilterMap(request, m_mDingDian_FilterField, reqmap);
	}
	else if (counterType == COUNTER_TYPE_JSD) // agc
	{
		GetFilterMap(request, m_mAGC_FilterField, reqmap);
	}
	else if (counterType == COUNTER_TYPE_XINYI || counterType == COUNTER_TYPE_TEST) // xinyi
	{
		GetFilterMap(request, m_mTest_FilterField, reqmap);
	}

	

	std::string sFilterRequest = "";
	for (std::map<std::string, std::string>::iterator it = reqmap.begin(); it != reqmap.end(); it++)
	{
		sFilterRequest += it->first;
		sFilterRequest += "=";
		sFilterRequest += it->second;
		sFilterRequest += SOH;
	}

	if (sFilterRequest.empty())
		sFilterRequest = request;


	// ��־Ŀ¼
	std::string sLogFileName = gConfigManager::instance().m_sLogFilePath;
	sLogFileName += "\\";

	std::string sysNo = log.sysno();
	if (sysNo.empty())
		sysNo = "no_sysno";
	sLogFileName += sysNo;
	sLogFileName += "\\";

	std::string sysVer = log.sysver();
	if (sysVer.empty())
		sysVer = "no_sysver";
	sLogFileName += sysVer;
	sLogFileName += "\\";

	std::string busiType = log.busitype();
	//0.ͨ��, 1.֤ȯ����, 2.������ȯ, 3.���Ͽ��� 4.�˻�ϵͳ, 5.ͳһ��֤�� 6.��Ȩ, 7.��ǩ
	if(busiType == "0")
	{
		busiType = "֤ȯ";
	}
	else if(busiType == "1")
	{
		busiType = "��Ʊ";
	}
	else if(busiType == "2")
	{
		busiType = "������ȯ";
	}
	else if(busiType == "3")
	{
		busiType = "����";
	}
	else if(busiType == "4")
	{
		busiType = "�˻�";
	}
	else if(busiType == "5")
	{
		busiType = "��֤";
	}
	else if(busiType == "6")
	{
		busiType = "��Ȩ";
	}
	else if(busiType == "7")
	{
		busiType = "��ǩ";
	}
	else
	{
		busiType = "no_busitype";
	}
	sLogFileName += busiType;
	sLogFileName += "\\";


	// ��ǰ����
	boost::gregorian::date day = boost::gregorian::day_clock::local_day();
	sLogFileName += boost::gregorian::to_iso_extended_string(day);
	sLogFileName += "\\";



	// ����Ŀ¼
	boost::filesystem::path p(sLogFileName);
	if (!boost::filesystem::exists(p))
	{
		boost::filesystem::create_directories(p);
	}

	// �˻�
	std::string account = log.account();
	if (account.empty())
		sLogFileName += "no_account";
	else
		sLogFileName += account;

	// ���ܺ�
	sLogFileName += "_" + funcid;

	/*
	//��־����
	switch (log.level())
	{
	case Trade::TradeLog::DEBUG_LEVEL:
		sLogFileName += "_debug.log";
		break;
	case Trade::TradeLog::INFO_LEVEL:
		sLogFileName += "_info.log";
		break;
	case Trade::TradeLog::WARN_LEVEL:
		sLogFileName += "_warn.log";
		break;
	case Trade::TradeLog::ERROR_LEVEL:
		sLogFileName += "_error.log";
		break;
	default:
		sLogFileName += "_nolevel.log";
	}
	*/

	if (log.status() == 0)
	{
		sLogFileName += "_error.log";
	}
	else
	{
		sLogFileName += "_success.log";
	}

	/*
	// ����д
	std::ofstream outfile(sLogFileName.c_str(), std::ios_base::app);
	if (outfile.is_open())
	{
		outfile << "���ܺ�: " << funcid << "\n";
		outfile << "�ͻ���IP��" << log.ip() << "\n";

		outfile << "�������ʱ��: " << log.recvtime() << "\n";
		outfile << "Ӧ����ʱ�䣺" << log.sendtime() << "\n";

		outfile << "����ʼ����ʱ�䣺" << log.begintime() << "\n";
		outfile << "ִ��ʱ��(����)��" << log.runtime()/1000 << "\n";
		
		outfile << "��̨��ַ��" << log.gtip()<< "\n";
		outfile << "��̨�˿ڣ�" << log.gtport()<< "\n";
		

		outfile << "�������ģ�" << log.enc_request() << "\n";
		// ��Ҫ���������ֶ�
		outfile << "������˺����ģ�" <<  sFilterRequest << "\n";


		outfile << "ִ�н����" << log.status() << "\n";
		outfile << "��̨�����룺" << log.retcode() << "\n";
		outfile << "��̨������Ϣ��" << log.retmsg() << "\n";

		// ��Ҫ���������ֶ�
		outfile << "Ӧ�����ģ�" << log.response() << "\n";
		outfile << "Ӧ�����ģ�" << log.enc_response() << "\n";
			
		// ��һ��
		outfile << "\n";

		outfile.close();
	}
	else
	{
		std::string sLog = "����־�ļ�" + sLogFileName + "ʧ��.";
		gFileLog::instance().Log(sLog);
	}
	*/

	HANDLE hFile = CreateFile(sLogFileName.c_str(), FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return true;
	}

	std::stringstream outfile;

	outfile << "���ܺ�: " << funcid << "\n";
		outfile << "�ͻ���IP��" << log.ip() << "\n";

		outfile << "�������ʱ��: " << log.recvtime() << "\n";
		outfile << "Ӧ����ʱ�䣺" << log.sendtime() << "\n";

		outfile << "����ʼ����ʱ�䣺" << log.begintime() << "\n";
		outfile << "ִ��ʱ��(����)��" << log.runtime()/1000 << "\n";
		
		outfile << "��̨��ַ��" << log.gtip()<< "\n";
		outfile << "��̨�˿ڣ�" << log.gtport()<< "\n";
		

		//outfile << "�������ģ�" << log.enc_request() << "\n";
		// ��Ҫ���������ֶ�
		outfile << "������˺����ģ�" <<  sFilterRequest << "\n";


		outfile << "ִ�н����" << log.status() << "\n";
		outfile << "��̨�����룺" << log.retcode() << "\n";
		outfile << "��̨������Ϣ��" << log.retmsg() << "\n";

		// ��Ҫ���������ֶ�
		outfile << "Ӧ�����ģ�" << log.response() << "\n";
		//outfile << "Ӧ�����ģ�" << log.enc_response() << "\n";
			
		// ��һ��
		outfile << "\n";

	DWORD dwBytesWritten = 0;

	std::string s = outfile.str();
	WriteFile(hFile, s.c_str(), s.length(), &dwBytesWritten, NULL);

	

	CloseHandle(hFile);

	// �ͷ�
	//log.destroy();
	

	return true;
}


/*
���ݹ����ֶζ���mapFieldFilter����������request�����ع��˺���reqmap
*/
void FileLogManager::GetFilterMap(std::string& request, std::map<std::string, std::string>& mapFieldFilter, std::map<std::string, std::string>& reqmap)
{
	std::string SOH = "\x01";
	std::vector<std::string> keyvalues;
	std::string keyvalue;
	std::string key = "";
	std::string value = "";

	boost::split(keyvalues, request, boost::is_any_of(SOH)); // ע����Ҫͨ�������ļ�����

	
	for (std::vector<std::string>::iterator it = keyvalues.begin(); it != keyvalues.end(); it++)
	{
		keyvalue = *it;
		

		if (keyvalue.empty())
			continue;

		std::size_t found = keyvalue.find_first_of("=");
		

		if (found != std::string::npos)
		{
			key = keyvalue.substr(0, found);
			value = keyvalue.substr(found + 1);
		
		}



		// ��������˹��˹ؼ��ֶ�
		if (gConfigManager::instance().m_nFilterField)
		{
			// ����
			if (mapFieldFilter.find(key) != mapFieldFilter.end())
				continue;
		}




		reqmap[key] = value;
	}
}

// �������ֶε������ļ�
void FileLogManager::LoadFieldFilter()
{
	std::string xmlfile = "";
	
	// ����
	xmlfile = gConfigManager::instance().m_sPath + "\\FilterField_DD.xml";
	LoadFieldFilter(xmlfile, m_mDingDian_FilterField);

	// ����T2
	xmlfile = gConfigManager::instance().m_sPath + "\\FilterField_HS.xml";
	LoadFieldFilter(xmlfile, m_mT2_FilterField);

	// ��֤
	xmlfile = gConfigManager::instance().m_sPath + "\\FilterField_JZ.xml";
	LoadFieldFilter(xmlfile, m_mKingdom_FilterField);

	// ���˴�
	xmlfile = gConfigManager::instance().m_sPath + "\\FilterField_JSD.xml";
	LoadFieldFilter(xmlfile, m_mAGC_FilterField);

	// �����̨
	xmlfile = gConfigManager::instance().m_sPath + "\\FilterField_Test.xml";
	LoadFieldFilter(xmlfile, m_mTest_FilterField);

}

// �����˲�ѯ���ܺŵ������ļ�
void FileLogManager::LoadFuncFilter()
{
	std::string xmlfile = "";
	
	// ����
	xmlfile = gConfigManager::instance().m_sPath + "\\FilterFunc_DD.xml";
	LoadFuncFilter(xmlfile, m_mDingDian_FilterFunc);

	// ����T2
	xmlfile = gConfigManager::instance().m_sPath + "\\FilterFunc_HS.xml";
	LoadFuncFilter(xmlfile, m_mT2_FilterFunc);

	// ��֤
	xmlfile = gConfigManager::instance().m_sPath + "\\FilterFunc_JZ.xml";
	LoadFuncFilter(xmlfile, m_mKingdom_FilterFunc);

	// ���ʴ�
	xmlfile = gConfigManager::instance().m_sPath + "\\FilterFunc_JSD.xml";
	LoadFuncFilter(xmlfile, m_mAGC_FilterFunc);

	// �����̨
	xmlfile = gConfigManager::instance().m_sPath + "\\FilterFunc_Test.xml";
	LoadFuncFilter(xmlfile, m_mTest_FilterFunc);
}

// ��xml�����ļ��ж�ȡ��Ҫ���˵������ֶ�
void FileLogManager::LoadFieldFilter(std::string& sFieldFilterXML, std::map<std::string, std::string>& mapFieldFilter)
{
	rapidxml::file<> f(sFieldFilterXML.c_str());

	rapidxml::xml_document<> doc;

	doc.parse<0>(f.data());

	rapidxml::xml_node<> * root = doc.first_node();


	

	rapidxml::xml_node<> * filter = root->first_node();
	while (filter != NULL)
	{
		//TRACE("name1 = %s\n", filter->name());

		rapidxml::xml_node<>* fields = filter->first_node();
		while (fields != NULL)
		{
			//TRACE("\tname2 = %s\n", fields->name());

			rapidxml::xml_node<>* field = fields->first_node();
			while (field != NULL)
			{
				//TRACE("\t\tname3 = %s\n", field->name());

				std::string sFieldName = field->value();
				mapFieldFilter[sFieldName] = ""; // value���������Ժ���չ��

				field = field->next_sibling();
			}

			fields = fields->next_sibling();
		} // end while

		filter = filter->next_sibling();
	} // end while
}

// ��xml�����ļ��ж�ȡ��Ҫ���˵Ĺ��ܺ�
void FileLogManager::LoadFuncFilter(std::string& sFuncFilterXML, std::map<std::string, FUNCTION_DESC>& mapFuncFilter)
{
	rapidxml::file<> f(sFuncFilterXML.c_str());

	rapidxml::xml_document<> doc;

	doc.parse<0>(f.data());

	rapidxml::xml_node<> * root = doc.first_node();

	
	

	rapidxml::xml_node<> * filter = root->first_node();
	while (filter != NULL)
	{
		//TRACE("name1 = %s\n", filter->name());
		

		rapidxml::xml_node<> * functions = filter->first_node();
		while (functions != NULL)
		{
			//TRACE("\tname2 = %s\n", functions->name());
			if (strcmp(functions->name(), "function") == 0)
			{

			//rapidxml::xml_node<> * function = functions->first_node();
			//while (function != NULL)
			//{
			//	TRACE("\t\tname3 = %s\n", function->name());

				FUNCTION_DESC funcDesc;

				std::string sFuncId = functions->first_attribute("id")->value();
				std::string sIsQuery = functions->first_attribute("isQuery")->value();
				std::string sHasResultSet = functions->first_attribute("hasResultSet")->value();
			
				if (sIsQuery == "true")
					funcDesc.isQuery = true;
				else
					funcDesc.isQuery = false;

				if (sHasResultSet == "true")
					funcDesc.hasResultRet = true;
				else
					funcDesc.hasResultRet = false;

				mapFuncFilter[sFuncId] = funcDesc;
			}

			//	function = function->next_sibling();
			//}

			functions = functions->next_sibling();
		}

		filter = filter->next_sibling();
	} // end while
}
