/*
 * IOmsClientManager.h
 *
 *  Created on: 2010-4-19
 *      Author: root
 */

#ifndef ICSCLIENTMANAGER_H_
#define ICSCLIENTMANAGER_H_

#include "ICSClient.h"

class ICSClientManager
{
public:
	ICSClientManager() {}
	virtual ~ICSClientManager() {}
	
	virtual void Release() =0;

	static ICSClientManager* CreateInstance();

	virtual bool Start( size_t thread_count=8, int request_timeout_ms=10000, int heart_interval=10, int heart_check_times=5) = 0;

	virtual ICSClient* CreateICSClient(const char* cAddr)=0;
};

#endif /* ICSCLIENTMANAGER_H_ */
