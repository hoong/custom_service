/*
 * OmsClientManager.cpp
 *
 *  Created on: 2010-4-19
 *      Author: root
 */

#include "CSClientManager.h"
//#include "command.h"
#include "cmd.h"


#define CONNECT_TIMEOUT_MS	500

ICSClientManager* ICSClientManager::CreateInstance()
{
	cs_client::CSClientManager* pMan = new cs_client::CSClientManager();
	if (pMan)
		return pMan;
	else
		return NULL;
}


namespace cs_client
{

CSClientManager::CSClientManager()
: m_request_timeout_ms(10000)
{
}

CSClientManager::~CSClientManager()
{
}

// request_timeout_ms 请求超时(ms), heart_interval:心跳检测(s)
bool CSClientManager::Start(size_t thread_count, int request_timeout_ms, int heart_interval, int heart_check_times)
{

	m_request_timeout_ms = request_timeout_ms;

	CS_CB_QUEUE::instance().SetTimeout(m_request_timeout_ms);
	
	//����reactor
	return CS_NET_MGR::instance().initialize();
}

ICSClient* CSClientManager::CreateICSClient(const char* cAddr)
{
	timeval tv;
	boost::shared_ptr<base::NetHandlerBase> spNetHandlerBase;

	if (!CS_NET_MGR::instance().connect(cAddr, &tv, spNetHandlerBase))
	{
		boost::shared_ptr<CSNetHandler> pCShandler= boost::dynamic_pointer_cast<CSNetHandler>(spNetHandlerBase);

		CSClient* pClient = new CSClient();
		pClient->pre_assign(this,  pCShandler);

		return pClient;
	}

	return NULL;
}


} /* end of namespace */
