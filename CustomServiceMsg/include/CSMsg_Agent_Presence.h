#ifndef CSMSG_AGENT_PRESENCE_H_
#define CSMSG_AGENT_PRESENCE_H_

#include "CSMsg_Datatype.h"
#include <boost/foreach.hpp>
#include "type.h"

struct CSMsg_Request_Agent_Presence
{
	CSMsg_Request_Agent_Presence():
		m_nqueue(0),m_ncapacity(0)
	{
		memset(&m_agent_mid,0,sizeof(UMID));
	}

	CSMsg_Request_Agent_Presence(const uint32& queue_id,const UMID& agent,const uint32&cap):
		m_nqueue(queue_id),m_ncapacity(cap),m_agent_mid(agent)
	{
	}


	template <typename IN>
	void decode(IN& inPacket)
	{
		inPacket >>this->m_nqueue >> this->m_agent_mid >>this->m_ncapacity;
	}

	template <typename OUT>
	void encode(OUT& outPacket)
	{
		outPacket <<this->m_nqueue<< this->m_agent_mid <<this->m_ncapacity;
	}

	uint32 m_nqueue;
	uint32 m_ncapacity;
	UMID m_agent_mid;

};


struct CSMsg_Response_Agent_Presence
{
	CSMsg_Response_Agent_Presence(uint32 queue_id):m_nqueue(queue_id){};
	CSMsg_Response_Agent_Presence():m_nqueue(0){};

	template <typename IN>
	void decode(IN& inPacket)
	{
		inPacket >> m_nqueue;
	}

	template <typename OUT>
	void encode(OUT& outPacket)
	{

		outPacket << m_nqueue;
		/*
		uint16 cnt = m_vec_invite.size();
		outPacket << cnt;

		BOOST_FOREACH(INVITE_INFO& elm,m_vec_invite)
		{
			outPacket<<elm.agent_mid.uid<<elm.agent_mid.multpt;
		}
		*/
	}

	uint32 m_nqueue;
	//std::vector<INVITE_INFO> m_vec_invite;
};



#endif
