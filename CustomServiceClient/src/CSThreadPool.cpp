#include "CSThreadPool.h"

namespace cs_client
{

bool CSThreadPool::initialize(uint8 num)
{
	//return 0 == base::ThreadPool::start(CONFIGURE_MGR::instance().m_mss_threadpool_size);
	return 0 == base::ThreadPool::start(num);
}

}

