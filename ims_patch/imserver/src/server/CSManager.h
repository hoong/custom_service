/*
 * CSManager.h
 *
 *  Created on: 2012-3-14
 *      Author: root
 */

#ifndef CSMANAGER_H_
#define CSMANAGER_H_

#include "ServerManager.h"
#include <ace/Singleton.h>
#include <ace/Null_Mutex.h>
#include "ICSClient.h"
#include "ICSClientManager.h"
#include "netbaselib/include/packet/TcpOutPacket.h"
#include "../Configure.h"

typedef CS_CONTEXT_TYPE CS_CONTEXT;


// 获取最优服务器
#define CS_GUARD(svr) \
	ICSClient* svr = NULL;\
	SERVER_GUARD(CCSManager, svr2, 0); \
	if (svr2 && svr2->m_pCSClient!=NULL) \
		svr = svr2->m_pCSClient; \


// CRefCSClient class

class CRefCSClient : public netbaselib::CRefThreadCounted<CRefCSClient>
{
public:
	CRefCSClient() : m_pCSClient(NULL) {}
	virtual ~CRefCSClient()
	{
		if (m_pCSClient)
		{
			m_pCSClient->Close();
			m_pCSClient->Release();
		}
	}

	bool IsValid() { return m_pCSClient ? m_pCSClient->IsConnected() : false; }

	void Close() { if (m_pCSClient) m_pCSClient->Close(); }

	void SetInfo(SVR_SESSION& sess)
	{
		m_sess = sess;
	}

	/*

	// 记录到个人离线消息
	int RecordPrivateMsgToOMS(uint64 uin, uint16 cmd, const void* pdata, size_t size,  REC_OMSG_CALLBACK cb, OMS_CONTEXT context)
	{
		ND_LOG_TRACE("RecordPrivateMsgToOMS: uin=%Q, cmd=%u, size=%u.", uin, cmd, size);

		if (m_pCSClient)
		{
			netbaselib::CTcpS2COutPacket outToOms;
			outToOms << cmd;
			outToOms.writeData((uint8*)pdata	, size);

			OFFLINE_MSG omsg;
			omsg.wSize = outToOms.getLength();
			omsg.lpMsgData = outToOms.getData();

			return m_pCSClient->RecordOfflineMsg(uin, omsg, cb, context);
		}
		return -1;
	}
	*/

public:
	SVR_SESSION m_sess;

	ICSClient* m_pCSClient;
};


// CCSManager class

class CCSManager;
typedef ACE_Singleton<CCSManager, ACE_Thread_Mutex> CS_MANAGER;

class CCSManager : public CServerManager<CRefCSClient*>
{
public:
	typedef ACE_Singleton<CCSManager, ACE_Thread_Mutex> singleton_type;
	friend class ACE_Singleton<CCSManager, ACE_Thread_Mutex>;

	//enum { E_SVR_TYPE = SVR_TYPE_CS }; // TODO
	enum { E_SVR_TYPE }; // TODO

	CCSManager();
	virtual ~CCSManager();

	bool Init();

	virtual CRefCSClient* CreateNewServer(SVR_SESSION& sess);

	void InitSession(SVR_SESSION* psess, uint32 uin)
	{
		psess->sid = 0;
		psess->addr = GetDefAddr();
	}

	const netbaselib::NET_ADDR& GetDefAddr() const
	{
		return CONFIGURE_MANAGER::instance()->m_cs_defaddr;
	}


private:
	ICSClientManager* m_pobjCSClientManager;
};



#endif
