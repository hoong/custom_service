#ifndef CSMSG_GET_MSG_H_
#define CSMSG_GET_MSG_H_

#include <boost/foreach.hpp>
#include "CSMsg_Datatype.h"
#include <string>


struct CSMsg_Request_Get_Msg
{
	CSMsg_Request_Get_Msg():
		m_nqueue(0),m_ncvsid(0)
	{
	}

	CSMsg_Request_Get_Msg(const uint32& queue_id,const uint64& cvs_id):
		m_nqueue(queue_id),m_ncvsid(cvs_id)
	{
	}


	template <typename IN>
	void decode(IN& inPacket)
	{
		inPacket>> this->m_nqueue >>this->m_ncvsid;
	}

	template <typename OUT>
	void encode(OUT& outPacket)
	{
		outPacket<< this->m_nqueue <<this->m_ncvsid;
	}


	uint32 m_nqueue;
	uint64 m_ncvsid;
};

struct CSMsg_Response_Get_Msg
{
	CSMsg_Response_Get_Msg(const uint64& queue_id,const uint64& cvs_id):
		m_nqueue(queue_id),m_ncvsid(cvs_id)
	{
	}

	CSMsg_Response_Get_Msg()
	{
	}

	template <typename IN>
	void decode(IN& inPacket)
	{
		inPacket>>m_nqueue>>m_ncvsid;
		uint16 cnt;
		inPacket >>cnt;
		for(uint16 idx = 0;idx<cnt;idx++)
		{
			MSG_INFO mi;
			inPacket>>mi;
			m_msg_list.push_back(mi);
		}
	}

	template <typename OUT>
	void encode(OUT& outPacket)
	{
		outPacket<<m_nqueue<<m_ncvsid<<base::packet::as_container(m_msg_list);

		/*
		uint16 cnt = m_msg_list.size();
		outPacket << cnt;
		BOOST_FOREACH(MSG_INFO& elm,m_msg_list)
		{
			outPacket<<elm.msg_id <<elm.agent <<elm.user
				<<elm.send_flag<<elm.send_time<<elm.send_text;
		}
		*/
	}


	//uint64 m_nuid;
	uint32 m_nqueue;
	uint64 m_ncvsid;
	std::vector<MSG_INFO> m_msg_list;
};

#endif
