#ifndef CSMSG_AGENT_GET_QUEUEAGENT_H_
#define CSMSG_AGENT_GET_QUEUEAGENT_H_

#include "type.h"
#include "CSMsg_Datatype.h"
//#include <boost/foreach.hpp>



struct CSMsg_Request_Agent_Get_QueueAgent
{
	CSMsg_Request_Agent_Get_QueueAgent():
		m_nqueue(0)
	{
	}

	CSMsg_Request_Agent_Get_QueueAgent(const uint32& queue_id):
		m_nqueue(queue_id)
	{
	}


	template <typename IN>
	void decode(IN& inPacket)
	{
		inPacket>> this->m_nqueue;
	}

	template <typename OUT>
	void encode(OUT& outPacket)
	{
		outPacket << this->m_nqueue;
	}

	uint32 m_nqueue;

};

struct CSMsg_Response_Agent_Get_QueueAgent
{
	CSMsg_Response_Agent_Get_QueueAgent()
	{
	}

	CSMsg_Response_Agent_Get_QueueAgent(uint32 queue_id):m_nqueue(queue_id)
	{
	}


	template <typename IN>
	void decode(IN& inPacket)
	{
		uint16 cnt;
		inPacket >> cnt;
		for (uint16 idx=0;idx<cnt;idx++)
		{
			AGENT_INFO elm;
			inPacket>>elm.mid.uin>>elm.mid.multptvalue>>elm.name;
			m_agent_list.push_back(elm);
		}
	}

	template <typename OUT>
	void encode(OUT& outPacket)
	{
		outPacket<<m_nqueue<<base::packet::as_container(m_agent_list);
		/*
		uint16 cnt = m_agent_list.size();
		outPacket << cnt;
		BOOST_FOREACH(AGENT_INFO& elm,m_agent_list)
		{
			outPacket<<elm.mid.uin<<elm.mid.multptvalue<<elm.name;
		}
		*/
	}

	uint32 m_nqueue;
	std::vector<AGENT_INFO> m_agent_list;

};

#endif

