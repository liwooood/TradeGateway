#ifndef _WORKER_
#define _WORKER_

#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

//#define BOOST_ALL_NO_LIB
#include <boost/thread.hpp>

#include "QueueThreadSafe.h"

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
				break;
			}
		}
	}
};
#endif
