#ifndef CSTHREADPOOL_H_
#define CSTHREADPOOL_H_
#include "thread_pool.h"
#include <boost/thread/detail/singleton.hpp>
#include "type.h"

namespace custom_service
{

class CSThreadPool : public base::ThreadPool
{
public:
	CSThreadPool()
	{
	}

	virtual ~CSThreadPool()
	{
	}

	bool initialize(uint8 num);

};

typedef boost::detail::thread::singleton<CSThreadPool> CS_THR_POOL;
typedef boost::detail::thread::singleton<CSThreadPool> CS_IMS_THR_POOL;

}

#endif
