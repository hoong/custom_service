/*
 * OMSClient.cpp
 *
 *  Created on: 2010-4-19
 *      Author: root
 */

#include "CSClient.h"
#include <vector>

namespace cs_client
{

CSClient::CSClient()
: m_pManager(NULL)
{
}

CSClient::~CSClient()
{
}


bool CSClient::IsConnected()
{
	return true;
}

int CSClient::Close()
{
	return 0;
}

int CSClient::Release()
{
	return 0;
}




// �û�������ѯ
int CSClient::Request_UserJoinMsg(
		uint32 nQueueid,
		uint64 nUserUid,
		uint64 nUserMultpt,
		uint32 nAddr,
		USER_JOIN_MSGS_CALLBACK cb,	//�ص�����
		CS_CONTEXT_TYPE dwContext)

{
	//ND_LOG_TRACE("[OMS] UserJoinMsg: nQueueid=%u.", nQueueid);
	printf("UserJoinMsg: nQueueid=%u.\n", nQueueid);

	if (!m_pManager)
		{
			//ND_LOG_ERROR("[OMS] UserDepartMsg: not connected!");
		printf("UserJoinMsg: not connected!\n");
			return -1;
		}

	uint32 index = CS_CB_QUEUE::instance().AddCallback(USER_JOIN_CALLBACK_TYPE, (void*)cb, dwContext, 0, 0, 0, 0);
	CSMsg_Request_User_Join req(nQueueid, nUserUid, nUserMultpt, nAddr);

	base::packet::OutPacket out;
	base::packet::Header header(WY_CS_USER_JOIN, 0, index);


	try
	{
		req.encode(out);
		header.body_size(out.get_size());
		if (m_cshandler->send_packet(header,out) == 0)
			return 0;
		else
			return -1;
	}
	catch(...)
	{
		//ND_LOG_ERROR("[OMS] UserJoinMsg: encode body fail!");
	}

	CS_CB_QUEUE::instance().RemoveCallback(index);
	return -1;
}

// �û�������ѯ
int CSClient::Request_UserDepartMsg(
		uint32 nQueueid,
		uint64 nUserUid,
		uint64 nUserMultpt,	
		USER_DEPART_MSGS_CALLBACK cb,	//�ص�����
		CS_CONTEXT_TYPE dwContext)

{
	//ND_LOG_TRACE("[OMS] UserDepartMsg: nQueueid=%u.", nQueueid);

	if (!m_pManager)
	{
		//ND_LOG_ERROR("[OMS] UserDepartMsg: not connected!");
		return -1;
	}

	uint32 index = CS_CB_QUEUE::instance().AddCallback(USER_DEPART_CALLBACK_TYPE, (void*)cb, dwContext, 0, 0, 0, 0);

	CSMsg_Request_User_Depart req(nQueueid, nUserUid, nUserMultpt);
	base::packet::OutPacket out;
	base::packet::Header header(WY_CS_USER_DEPART, 0, index);

	try
	{
		req.encode(out);
		header.body_size(out.get_size());
		if (m_cshandler->send_packet(header,out) == 0)
			return 0;
		else
			return -1;//ND_LOG_ERROR("[OMS] UserDepartMsg: send fail!");
	}
	catch(...)
	{
		//ND_LOG_ERROR("[OMS] UserDepartMsg: encode body fail!");
	}
	CS_CB_QUEUE::instance().RemoveCallback(index);
	return -1;
}

//�û�����
int CSClient::Request_UserScoreMsg(
		uint32 nQueueid,
		uint64 nCvsid,
		uint64 nUserUid,
		uint64 nUserMultpt,
		int8 nScore, //����
		USER_SCORE_MSGS_CALLBACK cb,
		CS_CONTEXT_TYPE dwContext)

{
	//ND_LOG_TRACE("[OMS] UserScoreMsg: nQueueid=%u, nScore=%d.", nQueueid, nScore);

	if (!m_pManager)
	{
		//ND_LOG_ERROR("[OMS] UserScoreMsg: not connected!");
		return -1;
	}

	uint32 index = CS_CB_QUEUE::instance().AddCallback(USER_SCORE_CALLBACK_TYPE, (void*)cb, dwContext, 0);

	CSMsg_Request_User_Score req(nQueueid,nCvsid, nUserUid, nUserMultpt, nScore);
	base::packet::OutPacket out;
	base::packet::Header header(WY_CS_USER_SCORE, 0, index);


	try
	{
		req.encode(out);
		header.body_size(out.get_size());
		if (m_cshandler->send_packet(header,out) == 0)
			return 0;
		else
			return -1;//ND_LOG_ERROR("[OMS] UserScoreMsg: send fail!");
	}
	catch (...)
	{
		//ND_LOG_ERROR("[OMS] UserScoreMsg: encode body fail!");
	}
	CS_CB_QUEUE::instance().RemoveCallback(index);
	return -1;
}


int CSClient::Request_Agent_Presence(
	uint32 nQueueid,
	uint64 nAgentUid,
	uint64 nAgentMultpt,
	uint32 nCapacity,
	AGENT_PRESENCE_CALLBACK cb,	//�ص�����
	CS_CONTEXT_TYPE dwContext)
{

	if (!m_pManager)
	{
		//ND_LOG_ERROR("[OMS] UserScoreMsg: not connected!");
		return -1;
	}

	uint32 index = CS_CB_QUEUE::instance().AddCallback(AGENT_PRESENCE_CALLBACK_TYPE, (void*)cb, dwContext, 0);

	UMID agent(nAgentUid,nAgentMultpt);
	
	CSMsg_Request_Agent_Presence req(nQueueid,agent,nCapacity);
	base::packet::OutPacket out;
	base::packet::Header header(WY_CS_AGENT_PRESENCE, 0, index);


	try
	{
		req.encode(out);
		header.body_size(out.get_size());
		if (m_cshandler->send_packet(header,out) == 0)
			return 0;
		else
			return -1;//ND_LOG_ERROR("[OMS] UserScoreMsg: send fail!");
	}
	catch (...)
	{
		//ND_LOG_ERROR("[OMS] UserScoreMsg: encode body fail!");
	}
	CS_CB_QUEUE::instance().RemoveCallback(index);
	return -1;

}

int CSClient::Request_Agent_Absence(
	uint32 nQueueid,
	uint64 nAgentUid,
	uint64 nAgentMultpt,
	AGENT_ABSENCE_CALLBACK cb,	//�ص�����
	CS_CONTEXT_TYPE dwContext)
{

	if (!m_pManager)
	{
		//ND_LOG_ERROR("[OMS] UserScoreMsg: not connected!");
		return -1;
	}

	uint32 index = CS_CB_QUEUE::instance().AddCallback(AGENT_ABSENCE_CALLBACK_TYPE, (void*)cb, dwContext, 0);

	UMID agent(nAgentUid,nAgentMultpt);
	
	CSMsg_Request_Agent_Absence req(nQueueid,agent);
	base::packet::OutPacket out;
	base::packet::Header header(WY_CS_AGENT_ABSENCE, 0, index);


	try
	{
		req.encode(out);
		header.body_size(out.get_size());
		if (m_cshandler->send_packet(header,out) == 0)
			return 0;
		else
			return -1;//ND_LOG_ERROR("[OMS] UserScoreMsg: send fail!");
	}
	catch (...)
	{
		//ND_LOG_ERROR("[OMS] UserScoreMsg: encode body fail!");
	}
	CS_CB_QUEUE::instance().RemoveCallback(index);

	return -1;
}


int CSClient::Request_Agent_Accept(
	uint32 nQueueid,
	uint64 nAgentUid,
	uint64 nAgentMultpt,
	AGENT_ACCEPT_CALLBACK cb,	//�ص�����
	CS_CONTEXT_TYPE dwContext)
{

	if (!m_pManager)
	{
		//ND_LOG_ERROR("[OMS] UserScoreMsg: not connected!");
		return -1;
	}

	uint32 index = CS_CB_QUEUE::instance().AddCallback(AGENT_ACCEPT_CALLBACK_TYPE, (void*)cb, dwContext, 0);

	UMID agent(nAgentUid,nAgentMultpt);
	
	CSMsg_Request_Agent_Accept req(nQueueid,agent);
	base::packet::OutPacket out;
	base::packet::Header header(WY_CS_AGENT_ACCEPT, 0, index);


	try
	{
		req.encode(out);
		if (m_cshandler->send_packet(header,out) == 0)
			return 0;
		else
			return -1;//ND_LOG_ERROR("[OMS] UserScoreMsg: send fail!");
	}
	catch (...)
	{
		//ND_LOG_ERROR("[OMS] UserScoreMsg: encode body fail!");
	}
	CS_CB_QUEUE::instance().RemoveCallback(index);


	return -1;
}
	
int CSClient::Request_Agent_Refuse(
	uint32 nQueueid,
	uint64 nAgentUid,
	uint64 nAgentMultpt,
	AGENT_REFUSE_CALLBACK cb,	//�ص�����
	CS_CONTEXT_TYPE dwContext)
{
	if (!m_pManager)
	{
		//ND_LOG_ERROR("[OMS] UserScoreMsg: not connected!");
		return -1;
	}

	uint32 index = CS_CB_QUEUE::instance().AddCallback(AGENT_REFUSE_CALLBACK_TYPE, (void*)cb, dwContext, 0);

	UMID agent(nAgentUid,nAgentMultpt);
	
	CSMsg_Request_Agent_Refuse req(nQueueid,agent);
	base::packet::OutPacket out;
	base::packet::Header header(WY_CS_AGENT_REFUSE, 0, index);


	try
	{
		req.encode(out);
		header.body_size(out.get_size());
		if (m_cshandler->send_packet(header,out) == 0)
			return 0;
		else
			return -1;//ND_LOG_ERROR("[OMS] UserScoreMsg: send fail!");
	}
	catch (...)
	{
		//ND_LOG_ERROR("[OMS] UserScoreMsg: encode body fail!");
	}
	CS_CB_QUEUE::instance().RemoveCallback(index);

	return -1;
}

int CSClient::Request_Agent_Redirect(
	uint32 nQueueid,
	uint32 nCvsid,
	uint64 nFromAgentUid,
	uint64 nFromAgentMultpt,
	uint64 nToAgentUid,
	uint64 nToAgentMultpt,
	AGENT_REDIRECT_CALLBACK cb,	//�ص�����
	CS_CONTEXT_TYPE dwContext)
{

	if (!m_pManager)
	{
		//ND_LOG_ERROR("[OMS] UserScoreMsg: not connected!");
		return -1;
	}

	uint32 index = CS_CB_QUEUE::instance().AddCallback(AGENT_REDIRECT_CALLBACK_TYPE, (void*)cb, dwContext, 0);

	UMID from_agent(nFromAgentUid,nFromAgentMultpt);
	UMID to_agent(nToAgentUid,nToAgentMultpt);
	
	CSMsg_Request_Agent_Redirect req(nQueueid,nCvsid,from_agent,to_agent);
	base::packet::OutPacket out;
	base::packet::Header header(WY_CS_AGENT_REDIRECT, 0, index);

	try
	{
		req.encode(out);
		header.body_size(out.get_size());
		if (m_cshandler->send_packet(header,out) == 0)
			return 0;
		else
			return -1;//ND_LOG_ERROR("[OMS] UserScoreMsg: send fail!");
	}
	catch (...)
	{
		//ND_LOG_ERROR("[OMS] UserScoreMsg: encode body fail!");
	}
	CS_CB_QUEUE::instance().RemoveCallback(index);


	return -1;
}

int CSClient::Request_Agent_Get_Queue(
	uint64 nAgentUid,
	AGENT_GET_QUEUE_CALLBACK cb,	//�ص�����
	CS_CONTEXT_TYPE dwContext)
{
	if (!m_pManager)
	{
		//ND_LOG_ERROR("[OMS] UserScoreMsg: not connected!");
		return -1;
	}

	uint32 index = CS_CB_QUEUE::instance().AddCallback(AGENT_GET_QUEUE_CALLBACK_TYPE, (void*)cb, dwContext, 0);

	
	CSMsg_Request_Agent_Get_Queue req(nAgentUid);
	base::packet::OutPacket out;
	base::packet::Header header(WY_CS_AGENT_GET_QUEUE, 0, index);


	try
	{
		req.encode(out);
		header.body_size(out.get_size());
		if (m_cshandler->send_packet(header,out) == 0)
			return 0;
		else
			return -1;//ND_LOG_ERROR("[OMS] UserScoreMsg: send fail!");
	}
	catch (...)
	{
		//ND_LOG_ERROR("[OMS] UserScoreMsg: encode body fail!");
	}
	CS_CB_QUEUE::instance().RemoveCallback(index);

	return -1;
}

int CSClient::Request_Agent_Get_QueueAgent(
	uint32 nQueueid,
	AGENT_GET_QUEUEAGENT_CALLBACK cb,	//�ص�����
	CS_CONTEXT_TYPE dwContext)
{
	if (!m_pManager)
	{
		//ND_LOG_ERROR("[OMS] UserScoreMsg: not connected!");
		return -1;
	}

	uint32 index = CS_CB_QUEUE::instance().AddCallback(AGENT_GET_QUEUEAGENT_CALLBACK_TYPE, (void*)cb, dwContext, 0);

	
	CSMsg_Request_Agent_Get_QueueAgent req(nQueueid);
	base::packet::OutPacket out;
	base::packet::Header header(WY_CS_AGENT_GET_QUEUEAGENT, 0, index);


	try
	{
		req.encode(out);
		header.body_size(out.get_size());
		if (m_cshandler->send_packet(header,out) == 0)
			return 0;
		else
			return -1;//ND_LOG_ERROR("[OMS] UserScoreMsg: send fail!");
	}
	catch (...)
	{
		//ND_LOG_ERROR("[OMS] UserScoreMsg: encode body fail!");
	}
	CS_CB_QUEUE::instance().RemoveCallback(index);


	return -1;
}

int CSClient::Request_Agent_Offline(
	uint32 nQueueid,
	uint64 nUserUid,
	uint64 nUserMultpt,
	AGENT_OFFLINE_CALLBACK cb,	//�ص�����
	CS_CONTEXT_TYPE dwContext)
{
	if (!m_pManager)
	{
		//ND_LOG_ERROR("[OMS] UserScoreMsg: not connected!");
		return -1;
	}

	uint32 index = CS_CB_QUEUE::instance().AddCallback(AGENT_OFFLINE_CALLBACK_TYPE, (void*)cb, dwContext, 0);

	UMID agent(nUserUid,nUserMultpt);
	
	CSMsg_Request_Agent_Offline req(nQueueid,agent);
	base::packet::OutPacket out;
	base::packet::Header header(WY_CS_AGENT_OFFLINE, 0, index);


	try
	{
		req.encode(out);
		header.body_size(out.get_size());
		if (m_cshandler->send_packet(header,out) == 0)
			return 0;
		else
			return -1;//ND_LOG_ERROR("[OMS] UserScoreMsg: send fail!");
	}
	catch (...)
	{
		//ND_LOG_ERROR("[OMS] UserScoreMsg: encode body fail!");
	}
	CS_CB_QUEUE::instance().RemoveCallback(index);


	return -1;
}

int CSClient::Request_Agent_UserNum(
	uint32 nQueueid,
	AGENT_USERAMOUNT_CALLBACK cb,	//�ص�����
	CS_CONTEXT_TYPE dwContext)
{
	if (!m_pManager)
	{
		//ND_LOG_ERROR("[OMS] UserScoreMsg: not connected!");
		return -1;
	}

	uint32 index = CS_CB_QUEUE::instance().AddCallback(AGENT_USERAMOUNT_CALLBACK_TYPE, (void*)cb, dwContext, 0);

	CSMsg_Request_Agent_Useramount req(nQueueid);
	base::packet::OutPacket out;
	base::packet::Header header(WY_CS_AGENT_USERAMOUNT, 0, index);


	try
	{
		req.encode(out);
		header.body_size(out.get_size());
		if (m_cshandler->send_packet(header,out) == 0)
			return 0;
		else
			return -1;//ND_LOG_ERROR("[OMS] UserScoreMsg: send fail!");
	}
	catch (...)
	{
		//ND_LOG_ERROR("[OMS] UserScoreMsg: encode body fail!");
	}
	CS_CB_QUEUE::instance().RemoveCallback(index);


	return -1;
}

int CSClient::Request_Conversation_End(
		uint32 nQueueid,
		uint64 nUserUid,
		uint64 nUserMultpt,
		uint64 nCvsid, //����
		CONVERSATION_END_CALLBACK cb,
		CS_CONTEXT_TYPE dwContext)
{
	if (!m_pManager)
	{
		//ND_LOG_ERROR("[OMS] UserScoreMsg: not connected!");
		return -1;
	}

	uint32 index = CS_CB_QUEUE::instance().AddCallback(CONVERSATION_END_CALLBACK_TYPE, (void*)cb, dwContext, 0);

	UMID user(nUserUid,nUserMultpt);
	CSMsg_Request_Conversation_End req(nQueueid,nCvsid,user);
	base::packet::OutPacket out;
	base::packet::Header header(WY_CS_CONVERSATION_END, 0, index);


	try
	{
		req.encode(out);
		header.body_size(out.get_size());
		if (m_cshandler->send_packet(header,out) == 0)
			return 0;
		else
			return -1;//ND_LOG_ERROR("[OMS] UserScoreMsg: send fail!");
	}
	catch (...)
	{
		//ND_LOG_ERROR("[OMS] UserScoreMsg: encode body fail!");
	}
	CS_CB_QUEUE::instance().RemoveCallback(index);


	return -1;
}



int CSClient::Request_Send_Msg(
	const uint32& nQueueid,
	const uint64& nCvsid,
	//const UMID& sender,
	const uint64& nSenderUid,
	const uint64& nSenderMultpt,
	const std::string& text,
	SEND_MSG_CALLBACK cb,
	CS_CONTEXT_TYPE dwContext)
{
	if (!m_pManager)
	{
		//ND_LOG_ERROR("[OMS] UserScoreMsg: not connected!");
		return -1;
	}
	UMID sender(nSenderUid,nSenderMultpt);

	uint32 index = CS_CB_QUEUE::instance().AddCallback(SEND_MSG_CALLBACK_TYPE, (void*)cb, dwContext, 0);

	CSMsg_Request_Send_Msg req(nQueueid,nCvsid,sender,text);
	base::packet::OutPacket out;
	base::packet::Header header(WY_CS_SEND_MSG, 0, index);


	try
	{
		req.encode(out);
		header.body_size(out.get_size());
		if (m_cshandler->send_packet(header,out) == 0)
			return 0;
		else
			return -1;//ND_LOG_ERROR("[OMS] UserScoreMsg: send fail!");
	}
	catch (...)
	{
		//ND_LOG_ERROR("[OMS] UserScoreMsg: encode body fail!");
	}
	CS_CB_QUEUE::instance().RemoveCallback(index);


	return -1;
}


int CSClient::Request_Get_Msg(
	const uint32& nQueueid,
	const uint64& nCvsid,
	GET_MSG_CALLBACK cb,
	CS_CONTEXT_TYPE dwContext)
{
	if (!m_pManager)
	{
		//ND_LOG_ERROR("[OMS] UserScoreMsg: not connected!");
		return -1;
	}

	uint32 index = CS_CB_QUEUE::instance().AddCallback(GET_MSG_CALLBACK_TYPE, (void*)cb, dwContext, 0);

	CSMsg_Request_Get_Msg req(nQueueid,nCvsid);
	base::packet::OutPacket out;
	base::packet::Header header(WY_CS_GET_MSG, 0, index);


	try
	{
		req.encode(out);
		header.body_size(out.get_size());
		if (m_cshandler->send_packet(header,out) == 0)
			return 0;
		else
			return -1;//ND_LOG_ERROR("[OMS] UserScoreMsg: send fail!");
	}
	catch (...)
	{
		//ND_LOG_ERROR("[OMS] UserScoreMsg: encode body fail!");
	}
	CS_CB_QUEUE::instance().RemoveCallback(index);


	return -1;
}




} /* end of namespace */
