#ifndef DISTRIBUTED_LOG_MANAGER_H
#define DISTRIBUTED_LOG_MANAGER_H


#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/thread/detail/singleton.hpp>

#include "QueueThreadSafe.h"
#include "WorkerThreadPool.h"

#include "tradelog.pb.h"

#include "common.h"


class DistributedLogManager
{
public:
	DistributedLogManager(void);
	~DistributedLogManager(void);

	typedef QueueThreadSafe<Trade::TradeLog> kafka_queue_type;
	typedef WorkerThreadPool<kafka_queue_type> kafka_worker_type;

	kafka_queue_type kafka_q_;
	kafka_worker_type kafka_worker_;

	void start();
	void stop();
	void push(Trade::TradeLog log);

	
	bool kafka_log(Trade::TradeLog log);

	std::string logFileName;
	

	// �����ֶ�
	std::map<std::string, std::string> m_mDingDian_FilterField;
	std::map<std::string, std::string> m_mT2_FilterField;
	std::map<std::string, std::string> m_mKingdom_FilterField;
	void LoadFieldFilter();
	void LoadFieldFilter(std::string& sFieldFilterXML, std::map<std::string, std::string>& mapFieldFilter);
	void GetFilterMap(std::string& request, std::map<std::string, std::string>& mapFieldFilter, std::map<std::string, std::string>& reqmap);

	// ���˹��ܺ�
	std::map<std::string, FUNCTION_DESC> m_mDingDian_FilterFunc;
	std::map<std::string, FUNCTION_DESC> m_mT2_FilterFunc;
	std::map<std::string, FUNCTION_DESC> m_mKingdom_FilterFunc;
	void LoadFuncFilter();
	void LoadFuncFilter(std::string& sFuncFilterXML, std::map<std::string, FUNCTION_DESC>& mapFuncFilter);
};

typedef boost::detail::thread::singleton<DistributedLogManager> gDistributedLogManager;


#endif
