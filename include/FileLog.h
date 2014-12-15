#ifndef FILE_LOG_H
#define FILE_LOG_H

#include <string>

#include <boost/thread/detail/singleton.hpp>

#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO 1
#define LOG_LEVEL_WARN 2
#define LOG_LEVEL_ERROR 3

class FileLog
{
public:
	FileLog(void);
	~FileLog(void);

	void Log(std::string log, int logLevel=0, std::string logFile="证券网关日志");

	void error(std::string logFile, std::string log);
	void debug(std::string logFile, std::string log);
	void info(std::string logFile, std::string log);
	void warn(std::string logFile, std::string log);
};

typedef boost::detail::thread::singleton<FileLog> gFileLog;

#endif