/*
 * OmsClientManager.h
 *
 *  Created on: 2010-4-19
 *      Author: root
 */

#ifndef OMSCLIENTMANAGER_H_
#define OMSCLIENTMANAGER_H_

#include <stdio.h>
#include <sys/time.h>
#include <boost/thread/detail/singleton.hpp>

#include "ICSClientManager.h"
#include "CSNetMgr.h"
#include "CSClient.h"
#include "CSThreadPool.h"
#include <boost/shared_ptr.hpp>

namespace cs_client
{
class CSClientManager :
	public ICSClientManager
{
public:
	CSClientManager();
	virtual ~CSClientManager();

	virtual void Release()
	{
		 CS_THR_POOL::instance().~ThreadPool();
	}

	virtual bool Start( size_t thread_count=8,int request_timeout_ms=10000, int heart_interval=10, int heart_check_times=5);

	virtual ICSClient* CreateICSClient(const char* cAddr);

public:
	time_t m_request_timeout_ms;
	
};

typedef boost::detail::thread::singleton<CSClientManager> CS_CLIENT_MGR;

} /* end of namespace */



#endif /* OMSCLIENTMANAGER_H_ */
