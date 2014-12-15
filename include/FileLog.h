#ifndef FILE_LOG_H
#define FILE_LOG_H

#include <string>

#include <boost/thread/detail/singleton.hpp>


class FileLog
{
public:
	FileLog(void);
	~FileLog(void);

	void Log(std::string log, int level=0, std::string file="证券网关日志");
	void error(std::string fileName, std::string log);
};

typedef boost::detail::thread::singleton<FileLog> gFileLog;

#endif