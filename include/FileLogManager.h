#ifndef FILE_LOG_MANAGER_H
#define FILE_LOG_MANAGER_H

#include <string>
#include <iostream>

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/thread/detail/singleton.hpp>


#include "QueueThreadSafe.h"
#include "WorkerThreadPool.h"
#include "tradelog.pb.h"
#include "common.h"



class FileLogManager
{
private:
	typedef QueueThreadSafe<Trade::TradeLog> file_queue_type;
	typedef WorkerThreadPool<file_queue_type> file_worker_type;

	file_queue_type file_q_;
	file_worker_type file_worker_;

	bool file_log(Trade::TradeLog log);

	
	void GetFilterMap(std::string& request, std::map<std::string, std::string>& mapFieldFilter, std::map<std::string, std::string>& reqmap);
	void LoadFieldFilter(std::string& sFieldFilterXML, std::map<std::string, std::string>& mapFieldFilter);

	

	void LoadFuncFilter(std::string& sFuncFilterXML, std::map<std::string, FUNCTION_DESC>& mapFuncFilter);

public:
	FileLogManager(void);
	~FileLogManager(void);

	

	void push(Trade::TradeLog log);
	void start();
	void stop();

	
	void LoadFieldFilter();
	void LoadFuncFilter();
	
	// �����ֶ�
	std::map<std::string, std::string> m_mDingDian_FilterField;
	std::map<std::string, std::string> m_mT2_FilterField;
	std::map<std::string, std::string> m_mKingdom_FilterField;
	std::map<std::string, std::string> m_mAGC_FilterField;
	std::map<std::string, std::string> m_mTest_FilterField;
	// ���˹��ܺ�
	std::map<std::string, FUNCTION_DESC> m_mDingDian_FilterFunc;
	std::map<std::string, FUNCTION_DESC> m_mT2_FilterFunc;
	std::map<std::string, FUNCTION_DESC> m_mKingdom_FilterFunc;
	std::map<std::string, FUNCTION_DESC> m_mAGC_FilterFunc;
	std::map<std::string, FUNCTION_DESC> m_mTest_FilterFunc;
	
};
typedef boost::detail::thread::singleton<FileLogManager> gFileLogManager;

#endif
