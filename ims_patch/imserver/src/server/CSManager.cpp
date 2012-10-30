/*
 * CSManager.cpp
 *
 *  Created on: 2010-8-30
 *      Author: root
 */

#include "CSManager.h"
#include <ace/INET_Addr.h>
#include "netbaselib/include/NetAddr.h"
#include "../Configure.h"


CCSManager::CCSManager()
: m_pobjCSClientManager(NULL)
{
}

CCSManager::~CCSManager()
{
	if (m_pobjCSClientManager)
		m_pobjCSClientManager->Release();
}

bool CCSManager::Init()
{
	m_pobjCSClientManager = ICSClientManager::CreateInstance();
	if (!m_pobjCSClientManager)
		return false;
	if (!m_pobjCSClientManager->Start(
			CONFIGURE_MANAGER::instance()->m_cs_threadpool_count,
			CONFIGURE_MANAGER::instance()->m_cs_request_timeout_ms,
			CONFIGURE_MANAGER::instance()->m_cs_heartbeat_interval,
			CONFIGURE_MANAGER::instance()->m_cs_heart_check_times
			))
		return false;

	SVR_SESSION sess;
	sess.sid = 0;
	sess.addr = CONFIGURE_MANAGER::instance()->m_cs_defaddr;
	if (CreateNewServer(sess))
	{
		m_bEnable = true;
		return true;
	}

	return false;
}

CRefCSClient* CCSManager::CreateNewServer(SVR_SESSION& sess)
{
	ND_LOG_DEBUG("CCSManager::CreateNewServer: sid=%d, addr=%s. \n", sess.sid, sess.addr.tostring());

	CRefCSClient* pClient;
	ACE_NEW_RETURN(pClient, CRefCSClient, false);

	pClient->SetInfo(sess);

	pClient->m_pCSClient = m_pobjCSClientManager->CreateICSClient(sess.addr.tostring());

	if (pClient->m_pCSClient)
	{
		ND_LOG_DEBUG("CCSManager::CreateNewServer Suceess! sid=%u, addr=%s", sess.sid, sess.addr.tostring());

		return Bind(sess, pClient);
	}

	ND_LOG_ERROR("CCSManager::CreateNewServer Fail! sid=%u, addr=%s", sess.sid, sess.addr.tostring());

	delete pClient;
	return NULL;
}

