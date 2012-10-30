#ifndef CSMSG_AGENT_GET_QUEUE_H_
#define CSMSG_AGENT_GET_QUEUE_H_

//#include <boost/foreach.hpp>
#include "CSMsg_Datatype.h"


struct CSMsg_Request_Agent_Get_Queue
{
	CSMsg_Request_Agent_Get_Queue():
		m_nuid(0)
	{
	}
	CSMsg_Request_Agent_Get_Queue(const uint64& uid):
		m_nuid(uid)
	{
	}


	template <typename IN>
	void decode(IN& inPacket)
	{
		inPacket>> this->m_nuid ;
	}

	template <typename OUT>
	void encode(OUT& outPacket)
	{
		outPacket << this->m_nuid ;
	}


	uint64 m_nuid;
};

struct CSMsg_Response_Agent_Get_Queue
{
	CSMsg_Response_Agent_Get_Queue(uint64 uid):
		m_nuid(uid)
	{
	}

	CSMsg_Response_Agent_Get_Queue()
	{
	}

	template <typename IN>
	void decode(IN& inPacket)
	{
		uint16 cnt;
		inPacket >> cnt;
		for (uint8_t idx=0;idx<cnt;idx++)
		{
			QUEUE_INFO elm;
			inPacket>>elm.wg_id >>elm.wg_name >>elm.queue_id>>elm.queue_name;
			m_queue_list.push_back(elm); 
		}

	}

	template <typename OUT>
	void encode(OUT& outPacket)
	{

		outPacket <<base::packet::as_container(m_queue_list);
		/*
		uint16 cnt = m_queue_list.size();
		outPacket << cnt;
		BOOST_FOREACH(QUEUE_INFO& elm,m_queue_list)
		{
			outPacket<<elm.wg_id <<elm.wg_name <<elm.queue_id<<elm.queue_name;
		}
		*/
	}

	std::vector<QUEUE_INFO> m_queue_list;

	uint64 m_nuid;
};

#endif
