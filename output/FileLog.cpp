#include "stdafx.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include "FileLog.h"
#include "configmanager.h"


FileLog::FileLog(void)
{
}


FileLog::~FileLog(void)
{
}


void FileLog::Log(std::string log, int logLevel, std::string logFile)
{
	TRACE("调试内容输出：%s\n", log.c_str());

	// 如果是运行模式，直接返回，不记录日志文件
	// 0error, 1warn, 2info, 3debug
	if (logLevel < gConfigManager::instance().m_nLogLevel)
		return;

	// 创建目录
	std::string sLogDir = gConfigManager::instance().m_sLogFilePath;

	boost::filesystem::path p(sLogDir);
	if (!boost::filesystem::exists(p))
	{
		boost::filesystem::create_directories(p);
	}

	std::string sLogFileName = "";
	if (logFile.empty())
	{
		sLogFileName = sLogDir + "\\证券网关日志";
	}
	else
	{
		std::size_t found = logFile.find_last_of("\\");
		
		std::string file = logFile;

		if (found != std::string::npos)
		{
			std::string subDir  = logFile.substr(0, found);
			file  = logFile.substr(found + 1);

			sLogDir += "\\" + subDir;

			boost::filesystem::path pp(sLogDir);
			if (!boost::filesystem::exists(pp))
			{
				boost::filesystem::create_directories(pp);
			}

		}
		
			
			

		sLogFileName = sLogDir + "\\" + file;
	}

	sLogFileName += "_";

	boost::gregorian::date day = boost::gregorian::day_clock::local_day();
	sLogFileName += to_iso_extended_string(day);
	sLogFileName += ".log";

	/*
	//std::ofstream outfile(sLogFileName.c_str(), std::ios_base::app);
	if (outfile.is_open())
	{
		boost::posix_time::ptime beginTime =  boost::posix_time::microsec_clock::local_time();
		std::string sBeginTime = boost::gregorian::to_iso_extended_string(beginTime.date()) + " " + boost::posix_time::to_simple_string(beginTime.time_of_day());
		outfile << "时间：" << sBeginTime << "\n";

		outfile << "内容：" << log << "\n";
			
		// 隔一行
		outfile << "\n";

		outfile.close();
	}
	*/
	HANDLE hFile = CreateFile(sLogFileName.c_str(), FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return;
	}

	boost::posix_time::ptime beginTime =  boost::posix_time::microsec_clock::local_time();
	std::string sBeginTime = boost::gregorian::to_iso_extended_string(beginTime.date()) + " " + boost::posix_time::to_simple_string(beginTime.time_of_day());
	
	std::stringstream ss;
	ss << "时间：" <<  sBeginTime  << "\n";
	ss << "内容：" << log << "\n";
	std::string s = ss.str();

	DWORD dwBytesWritten = 0;
	

	WriteFile(hFile, s.c_str(), s.size(), &dwBytesWritten, NULL);

	

	CloseHandle(hFile);

}


void FileLog::debug(std::string logFile, std::string log)
{
	Log(log, LOG_LEVEL_DEBUG, logFile);
}

void FileLog::info(std::string logFile, std::string log)
{
	Log(log, LOG_LEVEL_INFO, logFile);
}

void FileLog::warn(std::string logFile, std::string log)
{
	Log(log, LOG_LEVEL_WARN, logFile);
}

void FileLog::error(std::string logFile, std::string log)
{
	Log(log, LOG_LEVEL_ERROR, logFile);
}