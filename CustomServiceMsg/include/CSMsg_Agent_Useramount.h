#ifndef CSMSG_AGENT_USERAMOUNT_H_
#define CSMSG_AGENT_USERAMOUNT_H_

#include "type.h"

struct CSMsg_Request_Agent_Useramount
{
	CSMsg_Request_Agent_Useramount():
		m_nqueue(0)
	{
	}

	CSMsg_Request_Agent_Useramount(const uint32& queue_id):
		m_nqueue(queue_id)
	{
	}


	template <typename IN>
	void decode(IN& inPacket)
	{
		inPacket >>this->m_nqueue;
	}

	template <typename OUT>
	void encode(OUT& outPacket)
	{
		outPacket <<this->m_nqueue;
	}

	uint32 m_nqueue;

};

struct CSMsg_Response_Agent_Useramount
{
	CSMsg_Response_Agent_Useramount(const uint32& queue_id):m_nqueue(queue_id){};
	CSMsg_Response_Agent_Useramount():m_nqueue(0){};

	template<typename IN>
	void decode(IN& inPacket)
	{
		inPacket >> m_nqueue >> m_namount;
	}

	template<typename OUT>
	void encode(OUT& outPacket)
	{
		outPacket << m_nqueue << m_namount;
	}

	uint32 m_nqueue;
	uint16 m_namount;


};

#endif
