#ifndef CSIMSMSG_INVITE_LIST_H
#define CSIMSMSG_INVITE_LIST_H
#include <boost/foreach.hpp>
#include "CSMsg_Datatype.h"
#include "type.h"

struct CSIMSMsg_Request_Invite_List 
{
	CSIMSMsg_Request_Invite_List():
		m_nqueue(0)
	{
	}
	CSIMSMsg_Request_Invite_List(const uint32& queue_id):
		m_nqueue(queue_id)
	{
	}



	template <typename IN>
	void decode(IN& inPacket)
	{
		inPacket>>this->m_nqueue ;
		uint16 cnt(0);
		inPacket >>cnt;
		for(uint16 i =0;i<cnt;i++)
		{
			USER_MULTPT_ID id;
			inPacket >>id.uin >>id.multptvalue;
			m_invite_list.push_back(id);
		}
		
	}

	template <typename OUT>
	void encode(OUT& outPacket)
	{
		outPacket <<this->m_nqueue;

		uint16 cnt(0) ;
		outPacket << cnt;
		BOOST_FOREACH(USER_MULTPT_ID& elm,m_invite_list)
		{
			outPacket<<elm.uin<<elm.multptvalue;
		}


	}

	uint32 m_nqueue;
	std::vector<USER_MULTPT_ID> m_invite_list;

};

struct CSIMSMsg_Response_Invite_List 
{
	CSIMSMsg_Response_Invite_List (uint32 queue_id):m_nqueue(queue_id)
	{
	}

	CSIMSMsg_Response_Invite_List ()
	{
	}

	template <typename IN>
	void decode(IN& inPacket)
	{
	}

	template <typename OUT>
	void encode(OUT& outPacket)
	{
	}

	uint32 m_nqueue;


};

#endif
