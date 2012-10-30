#ifndef CSIMSMSG_USER_LIST_H
#define CSIMSMSG_USER_LIST_H
#include <boost/foreach.hpp>
#include "CSMsg_Datatype.h"
#include "type.h"


struct CSIMSMsg_Request_User_List 
{
	CSIMSMsg_Request_User_List():
		m_nqueue(0)
	{
	}
	CSIMSMsg_Request_User_List(const uint32& queue_id):
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
			USER_INFO ui;
			inPacket >>ui.mid.uin >>ui.mid.multptvalue >>ui.position;
			m_user_list.push_back(ui);
		}
		
	}

	template <typename OUT>
	void encode(OUT& outPacket)
	{
		outPacket <<this->m_nqueue;

		uint16 cnt(0);
		outPacket << cnt;
		BOOST_FOREACH(USER_INFO& elm,m_user_list)
		{
			outPacket<<elm.mid.uin<<elm.mid.multptvalue<<elm.position;
		}


	}

	uint32 m_nqueue;
	std::vector<USER_INFO> m_user_list;

};

struct CSIMSMsg_Response_User_List 
{
	CSIMSMsg_Response_User_List (uint32 queue_id):m_nqueue(queue_id)
	{
	}

	CSIMSMsg_Response_User_List ()
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
