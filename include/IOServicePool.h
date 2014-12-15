#ifndef IO_SERVICE_POOL_H
#define IO_SERVICE_POOL_H

#include <algorithm>

#include <boost/assert.hpp>
#include <boost/noncopyable.hpp>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/functional/factory.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

// define BOOST_ALL_NO_LIB
#include <boost/asio.hpp>
#include <boost/thread.hpp>

/*
实现可以参考
http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/example/cpp03/http/server2/io_service_pool.hpp
http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/example/cpp03/http/server2/io_service_pool.cpp
*/

class IOServicePool : boost::noncopyable
{
public:
	typedef boost::asio::io_service IOSType;
	typedef boost::asio::io_service::work WorkType;

	typedef boost::ptr_vector<IOSType> IOServicesType;
	typedef boost::ptr_vector<WorkType> WorksType;

private:
	IOServicesType ioservices;
	WorksType works;

	boost::thread_group threads;
	
	std::size_t nextIOService;

public:
	//n应该设置为cpu核心数
	explicit IOServicePool(int IOThreadNum=4)
		:nextIOService(0)
	{
		BOOST_ASSERT(IOThreadNum > 0);
		init(IOThreadNum);
	}

private:
	void init(int IOThreadNum)
	{
		for (int i=0; i<IOThreadNum; ++i)
		{
			ioservices.push_back(boost::factory<IOSType*>()());

			works.push_back( boost::factory<WorkType*>()(ioservices.back()) );
		}
	}

public:
	IOSType& get()
	{
		if (nextIOService >= ioservices.size())
			nextIOService = 0;

		return ioservices[nextIOService++];
	}

public:
	void start()
	{
		if (threads.size() > 0)
			return;

		for(IOSType& ios : ioservices)
		{
			threads.create_thread(boost::bind(&IOSType::run, boost::ref(ios)));
			//boost::thread * thread = threads_.create_thread(boost::bind(&ios_type::run, boost::ref(ios)));
			//threads.push_back(thread);
		}
	}

	void run()
	{
		start();
		threads.join_all();
	}

	void stop()
	{
		works.clear();

		std::for_each(ioservices.begin(), ioservices.end(), boost::bind(&IOSType::stop, _1));
	}
};
#endif
