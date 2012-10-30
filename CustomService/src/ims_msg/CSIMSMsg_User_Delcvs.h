#ifndef CSIMSMSG_USERCVS_LIST_H
#define CSIMSMSG_USERCVS_LIST_H
#include "CSMsg_Datatype.h"
#include "type.h"
#include <boost/foreach.hpp>

struct CSIMSMsg_Request_User_Delcvs
{
	CSIMSMsg_Request_User_Delcvs():
		m_nqueue(0)
	{
	}
	CSIMSMsg_Request_User_Delcvs(const uint32& queue_id):
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
			USER_CONVERSATION ucs;
			inPacket >>ucs.mid.uin >>ucs.mid.multptvalue >>ucs.conversation_id;
			m_usercvs_list.push_back(ucs);
		}

	}

	template <typename OUT>
	void encode(OUT& outPacket)
	{
		outPacket <<this->m_nqueue;

		uint16 cnt(0) ;
		outPacket << cnt;
		BOOST_FOREACH(USER_CONVERSATION& elm,m_usercvs_list)
		{
			outPacket<<elm.mid.uin<<elm.mid.multptvalue<<elm.conversation_id;
		}


	}

	uint32 m_nqueue;
	std::vector<USER_CONVERSATION> m_usercvs_list;

};

struct CSIMSMsg_Response_Usercvs_List 
{
	CSIMSMsg_Response_Usercvs_List (uint32 queue_id):m_nqueue(queue_id)
	{
	}

	CSIMSMsg_Response_Usercvs_List ()
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
