#ifndef CSMSG_CONVERSATION_END_H_
#define CSMSG_CONVERSATION_END_H_

#include "CSMsg_Datatype.h"
#include "type.h"

struct CSMsg_Request_Conversation_End
{
	CSMsg_Request_Conversation_End():
		m_nqueue(0),m_ncvsid(0)
	{
		memset(&m_user_mid,0,sizeof(UMID));
	}

	CSMsg_Request_Conversation_End(const uint32& queue_id,const uint64& cvs_id,const UMID& user):
		m_nqueue(queue_id),m_ncvsid(cvs_id),m_user_mid(user)
	{
	}



	template <typename IN>
	void decode(IN& inPacket)
	{
		inPacket>>this->m_nqueue >>this->m_ncvsid>> this->m_user_mid;
	}

	template <typename OUT>
	void encode(OUT& outPacket)
	{
		outPacket <<this->m_nqueue<<this->m_ncvsid<< this->m_user_mid;
	}

	uint32 m_nqueue;
	uint64 m_ncvsid;
	UMID m_user_mid;

};

struct CSMsg_Response_Conversation_End
{
	CSMsg_Response_Conversation_End(uint32 queue_id):m_nqueue(queue_id)
	{
	}

	CSMsg_Response_Conversation_End()
	{
	}

	template <typename IN>
	void decode(IN& inPacket)
	{
		inPacket >>m_nqueue;
	}

	template <typename OUT>
	void encode(OUT& outPacket)
	{
		//uint16 cnt ;
		outPacket <<m_nqueue;
		/*<<m_agent_mid.uin<<m_agent_mid.multptvalue
			<<m_usercvs.mid.uin <<m_usercvs.mid.multptvalue <<m_usercvs.conversation_id;
			*/

		/*
		cnt = m_vec_invite.size();
		outPacket << cnt;

		BOOST_FOREACH(INVITE_INFO& elm,m_vec_invite)
		{
			outPacket<<elm.agent_mid.uin<<elm.agent_mid.multptvalue;
		}
		*/

	}

	uint32 m_nqueue;
	/*
	UMID m_agent_mid;
	USER_CONVERSATION m_usercvs;
	*/

//	std::vector<INVITE_INFO> m_vec_invite;

};

#endif
