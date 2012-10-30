#ifndef CSMSG_AGENT_ABSENCE_H_
#define CSMSG_AGENT_ABSENCE_H_

#include "type.h"

struct CSMsg_Request_Agent_Absence
{
	CSMsg_Request_Agent_Absence():
		m_nqueue(0)
	{
		memset(&m_agent_mid,0,sizeof(UMID));
	}
	CSMsg_Request_Agent_Absence(const uint32& queue_id,const UMID& agent):
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

struct CSMsg_Response_Agent_Absence
{
	CSMsg_Response_Agent_Absence(uint32 queue_id):m_nqueue(queue_id){};
	CSMsg_Response_Agent_Absence():m_nqueue(0){};
	template<typename IN>
	void decode(IN& inPacket)
	{
		inPacket>>m_nqueue;
	}

	template<typename OUT>
	void encode(OUT& outPacket)
	{
		outPacket<<m_nqueue;
	}

	uint32 m_nqueue;

};

#endif
