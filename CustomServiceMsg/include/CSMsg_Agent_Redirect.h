#ifndef CSMSG_AGENT_REDIRECT_H_
#define CSMSG_AGENT_REDIRECT_H_

#include "type.h"
#include "CSMsg_Datatype.h"

struct CSMsg_Request_Agent_Redirect
{
	CSMsg_Request_Agent_Redirect():
		m_nqueue(0),m_ncvsid(0)
	{
		memset(&m_agent_from,0,sizeof(UMID));
		memset(&m_agent_to,0,sizeof(UMID));
	}

	CSMsg_Request_Agent_Redirect(
		const uint32& queue_id,
		const uint64& cvs_id,
		const UMID& from,
		const UMID& to):
		m_nqueue(queue_id),m_ncvsid(cvs_id),m_agent_from(from),m_agent_to(to)
	{
	}


	template <typename IN>
	void decode(IN& inPacket)
	{
		inPacket  >> this->m_nqueue>>this->m_ncvsid >> this->m_agent_from >> this->m_agent_to;
	}

	template <typename OUT>
	void encode(OUT& outPacket)
	{
		outPacket<< this->m_nqueue<<this->m_ncvsid <<this->m_agent_from <<this->m_agent_to;
	}

	uint32 m_nqueue;
	uint64 m_ncvsid;
	UMID m_agent_from;
	UMID m_agent_to;
};

struct CSMsg_Response_Agent_Redirect
{
	CSMsg_Response_Agent_Redirect(uint32 queue_id):m_nqueue(queue_id){};
	CSMsg_Response_Agent_Redirect():m_nqueue(0){};

	template<typename IN>
	void decode(IN& inPacket)
	{
		inPacket>>m_nqueue;
	}

	template<typename OUT>
	void encode(OUT& outPacket)
	{
		outPacket<<m_nqueue;
		/*
		uint16 cnt = m_vec_invite.size();
		outPacket << cnt;

		BOOST_FOREACH(INVITE_INFO& elm,m_vec_invite)
		{
			outPacket<<elm.agent_mid.uin<<elm.agent_mid.multptvalue;
		}
		*/
	}

	uint32 m_nqueue;
	//std::vector<INVITE_INFO> m_vec_invite;

};

#endif
