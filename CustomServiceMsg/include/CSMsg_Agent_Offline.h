#ifndef CSMSG_AGENT_OFFLINE_H_
#define CSMSG_AGENT_OFFLINE_H_

#include "type.h"
#include <boost/foreach.hpp>


struct CSMsg_Request_Agent_Offline
{
	CSMsg_Request_Agent_Offline():
		m_nqueue(0)
	{
		memset(&m_agent_mid,0,sizeof(UMID));
	}

	CSMsg_Request_Agent_Offline(const uint32& queue_id,const UMID& agent):
		m_nqueue(queue_id),m_agent_mid(agent)
	{
	}


	template <typename IN>
	void decode(IN& inPacket)
	{
		inPacket >>this->m_nqueue >> this->m_agent_mid;
	}

	template <typename OUT>
	void encode(OUT& outPacket)
	{
		outPacket <<this->m_nqueue<< this->m_agent_mid;
	}

	uint32 m_nqueue;
	UMID m_agent_mid;

};

struct CSMsg_Response_Agent_Offline
{
	CSMsg_Response_Agent_Offline(uint32 queue_id):m_nqueue(queue_id){};
	CSMsg_Response_Agent_Offline():m_nqueue(0){};
	template<typename IN>
	void decode(IN& inPacket)
	{

		inPacket >> m_nqueue;
		return;
	}

	template<typename OUT>
	void encode(OUT& outPacket)
	{
		//uint16 cnt;

		outPacket << m_nqueue;

		/*
		cnt = m_vec_usercvs.size();
		outPacket << cnt;

		BOOST_FOREACH(USER_CONVERSATION& elm,m_vec_usercvs)
		{
			outPacket<<elm.mid.uin<<elm.mid.multpt<<elm.conversation_id;
		}

		cnt = m_vec_invite.size();
		outPacket << cnt;

		BOOST_FOREACH(INVITE_INFO& elm,m_vec_invite)
		{
			outPacket<<elm.agent_mid.uin<<elm.agent_mid.multpt;
		}
		*/

	}


	uint32 m_nqueue;
	/*
	std::vector<USER_CONVERSATION> m_vec_usercvs;
	std::vector<INVITE_INFO> m_vec_invite;
	*/

};

#endif
