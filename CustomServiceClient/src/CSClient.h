/*
 * OMSClient.h
 *
 *  Created on: 2012-2-19
 *      Author: root
 */

#ifndef CSCLIENT_H_
#define CSCLIENT_H_

#include <boost/thread/detail/singleton.hpp>
#include "ICSClient.h"
#include "CSNetHandler.h"
#include "CallbackQueue.h"
#include "CSNetHandler.h"
//#include "command.h"
#include "cmd.h"
//msg define
#include "CSMsg_Datatype.h"
#include "CSMsg_Agent_Absence.h"
#include "CSMsg_Agent_Accept.h"
#include "CSMsg_Agent_Get_QueueAgent.h"
#include "CSMsg_Agent_Get_Queue.h"
#include "CSMsg_Agent_Offline.h"
#include "CSMsg_Agent_Presence.h"
#include "CSMsg_Agent_Redirect.h"
#include "CSMsg_Agent_Refuse.h"
#include "CSMsg_Agent_Useramount.h"
#include "CSMsg_Conversation_End.h"
#include "CSMsg_User_Depart.h"
#include "CSMsg_User_Join.h"
#include "CSMsg_User_Score.h"
#include "CSMsg_Send_Msg.h"
#include "CSMsg_Get_Msg.h"
//packet
#include "packet/in_packet.h"
#include "packet/out_packet.h"
#include "packet/header.h"



namespace cs_client
{

class CSClientManager;

class CSClient:
	public ICSClient
{

public:
	CSClient();
	virtual ~CSClient();

	virtual bool IsConnected();
	virtual int Close();
	virtual int Release();

public:
		virtual int Request_Agent_Presence(
		uint32 nQueueid,
		uint64 nAgentUid,
		uint64 nAgentMultpt,
		uint32 nCapacity,
		AGENT_PRESENCE_CALLBACK cb,	//�ص�����
		CS_CONTEXT_TYPE dwContext);
	
	virtual int Request_Agent_Absence(
		uint32 nQueueid,
		uint64 nAgentUid,
		uint64 nAgentMultpt,
		AGENT_ABSENCE_CALLBACK cb,	//�ص�����
		CS_CONTEXT_TYPE dwContext);

	virtual int Request_Agent_Accept(
		uint32 nQueueid,
		uint64 nAgentUid,
		uint64 nAgentMultpt,
		AGENT_ACCEPT_CALLBACK cb,	//�ص�����
		CS_CONTEXT_TYPE dwContext);

	virtual int Request_Agent_Refuse(
		uint32 nQueueid,
		uint64 nAgentUid,
		uint64 nAgentMultpt,
		AGENT_REFUSE_CALLBACK cb,	//�ص�����
		CS_CONTEXT_TYPE dwContext);

	virtual int Request_Agent_Redirect(
		uint32 nQueueid,
		uint32 nCvsid,
		uint64 nFromAgentUid,
		uint64 nFromAgentMultpt,
		uint64 nToAgentUid,
		uint64 nToAgentMultpt,
		AGENT_REDIRECT_CALLBACK cb,	//�ص�����
		CS_CONTEXT_TYPE dwContext);

	virtual int Request_Agent_Get_Queue(
		uint64 nAgentUid,
		AGENT_GET_QUEUE_CALLBACK cb,	//�ص�����
		CS_CONTEXT_TYPE dwContext);
	
	virtual int Request_Agent_Get_QueueAgent(
		uint32 nQueueid,
		AGENT_GET_QUEUEAGENT_CALLBACK cb,	//�ص�����
		CS_CONTEXT_TYPE dwContext);

	virtual int Request_Agent_Offline(
		uint32 nQueueid,
		uint64 nUserUid,
		uint64 nUserMultpt,
		AGENT_OFFLINE_CALLBACK cb,	//�ص�����
		CS_CONTEXT_TYPE dwContext);

	virtual int Request_Agent_UserNum(
		uint32 nQueueid,
		AGENT_USERAMOUNT_CALLBACK cb,	//�ص�����
		CS_CONTEXT_TYPE dwContext);

	

	// �û�������ѯ
	virtual int Request_UserJoinMsg(
		uint32 nQueueid,
		uint64 nUserUid,
		uint64 nUserMultpt,
		uint32 nAddr,
		USER_JOIN_MSGS_CALLBACK cb,	//�ص�����
		CS_CONTEXT_TYPE dwContext);

	// �û�������ѯ
	virtual int Request_UserDepartMsg(
		uint32 nQueueid,
		uint64 nUserUid,
		uint64 nUserMultpt,
		USER_DEPART_MSGS_CALLBACK cb,	//�ص�����
		CS_CONTEXT_TYPE dwContext);

	//�û�����
	virtual int Request_UserScoreMsg(
		uint32 nQueueid,
		uint64 nCvsid,
		uint64 nUserUid,
		uint64 nUserMultpt,
		int8 nScore, //����
		USER_SCORE_MSGS_CALLBACK cb,
		CS_CONTEXT_TYPE dwContext);

	virtual int Request_Conversation_End(
		uint32 nQueueid,
		uint64 nUserUid,
		uint64 nUserMultpt,
		uint64 nCvsid, //����
		CONVERSATION_END_CALLBACK cb,
		CS_CONTEXT_TYPE dwContext);

	virtual int Request_Send_Msg(
		const uint32& nQueueid,
		const uint64& nCvsid,
		//const UMID& sender,
		const uint64& nSenderUid,
		const uint64& nSenderMultpt,
		const std::string& text,
		SEND_MSG_CALLBACK cb,
		CS_CONTEXT_TYPE dwContext);

	virtual int Request_Get_Msg(
		const uint32& nQueueid,
		const uint64& nCvsid,
		GET_MSG_CALLBACK cb,
		CS_CONTEXT_TYPE dwContext);



	void pre_assign(CSClientManager* pManager, boost::shared_ptr<CSNetHandler>& handler)
	{
		m_pManager = pManager;
		m_cshandler = handler;
	}

private:
	CSClientManager* m_pManager;
	boost::shared_ptr<CSNetHandler> m_cshandler;

};

typedef boost::detail::thread::singleton<CSClient> CS_CLIENT;

} /* end of namespace */

#endif /* OMSCLIENT_H_ */
