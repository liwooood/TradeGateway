#ifndef WORKER_THREADPOOL_H
#define WORKER_THREADPOOL_H

#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

//#define BOOST_ALL_NO_LIB
#include <boost/thread.hpp>

#include "QueueThreadSafe.h"
#include "FileLog.h"


template<typename Queue>
class WorkerThreadPool
{
public:
	typedef Queue QueueType;
	typedef typename Queue::MsgType MsgType;
	typedef boost::function<bool(MsgType&)> FuncType;

private:
	QueueType& queue;
	FuncType func;

	int workThreadNum;
	boost::thread_group threads;
	//std::list<boost::thread*> threads;

public:
	template<typename Func>
	WorkerThreadPool(QueueType& q, Func f, int n = 1)
		:queue(q), func(f), workThreadNum(n)
	{
		BOOST_ASSERT(n>0);
	}

	WorkerThreadPool(QueueType& q, int n=1):
		queue(q), workThreadNum(n)
	{
		BOOST_ASSERT(n>0);
	}

public:
	void start()
	{
		BOOST_ASSERT(func);

		if (threads.size() > 0)
			return;

		for (int i=0; i<workThreadNum; ++i)
		{
			threads.create_thread(boost::bind(&WorkerThreadPool::do_work, this));
		}
	}

	template<typename Func>
	void start(Func f)
	{
		func = f;
		start();
	}

	void run()
	{
		start();
		threads.join_all();
	}




	void stop()
	{
		func = 0;
		queue.stop();

		/*
		for(std::list<boost::thread*>::iterator it = threads.begin(), end=threads.end(); it!=end; ++it)
		{
			threads_.remove_thread(*it);

			if (*it != NULL)
				delete *it;
		}
		*/
	}

private:
	void do_work()
	{
		for (;;)
		{
			MsgType msg = queue.pop();

			if (!func || !func(msg))
			{
				// 严重错误，需要记录日志
				//error("工作线程池退出线程");
				break;
			}
		}
	}

	
void error(std::string log)
{
	
	// 创建目录
	std::string sLogDir = gConfigManager::instance().m_sLogFilePath;

	
	std::string sLogFileName = sLogDir + "\\证券网关日志";
	
	sLogFileName += "_";

	boost::gregorian::date day = boost::gregorian::day_clock::local_day();
	sLogFileName += to_iso_extended_string(day);
	sLogFileName += ".log";

	
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

};
#endif
