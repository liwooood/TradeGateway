#ifndef QUEUE_THREAD_SAFE_H
#define QUEUE_THREAD_SAFE_H

#include <deque>

#include <boost/noncopyable.hpp>
#include <boost/utility/value_init.hpp>

#include <boost/assert.hpp>
#include <boost/concept_check.hpp>

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>


template<typename Msg>
class QueueThreadSafe : boost::noncopyable
{
public:
	typedef Msg MsgType;
	typedef std::deque<MsgType> QueueType;

	typedef boost::mutex MutexType;
	typedef boost::unique_lock<MutexType> LockType;
	//typedef typename mutex_type::scoped_lock lock_type;
	typedef boost::condition_variable_any ConditionType;

	// 要求有拷贝构造函数和重载operator=
	BOOST_CONCEPT_ASSERT((boost::SGIAssignable<MsgType>));
	// 要求有缺省构造函数
	BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<MsgType>));

	QueueType queue;

private:
	MutexType mutex;
	ConditionType condition;
	bool stopFlag;

public:
	QueueThreadSafe(void)
		:stopFlag(false)
	{
	}

	~QueueThreadSafe(void)
	{
	}
	

	void push(const MsgType& msg)
	{
		LockType lock(mutex);

		queue.push_back(msg);

		condition.notify_one();
	}


	MsgType pop()
	{
		LockType lock(mutex);

		while(queue.empty() && !stopFlag)
		{
			condition.wait(mutex);
		}

		if (stopFlag)
		{
			return MsgType();
				//return boost::initialized_value;
		}

		BOOST_ASSERT(!queue.empty());

		MsgType msg = queue.front();
		queue.pop_front();

		return msg;
	}

	void stop()
	{
		stopFlag = true;

		condition.notify_all();
	}
};
#endif
