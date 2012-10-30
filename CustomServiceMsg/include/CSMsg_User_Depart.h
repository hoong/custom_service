#ifndef CSMSG_USER_DEPART_H_
#define CSMSG_USER_DEPART_H_

#include "type.h"
#include "CSMsg_Datatype.h"
#include <boost/foreach.hpp>


struct CSMsg_Request_User_Depart
{
	CSMsg_Request_User_Depart():
		m_nqueue(0)
	{
		memset(&m_user_mid,0,sizeof(UMID));
	}

	CSMsg_Request_User_Depart(uint32 nQueueid, uint64 nUserUid, uint64 nUserMultpt):
		m_nqueue(nQueueid)
	{
		m_user_mid.uin = nUserUid;
		m_user_mid.multptvalue = nUserMultpt;
	}


	template <typename IN>
	void decode(IN& inPacket)
	{
		inPacket>>this->m_nqueue>> this->m_user_mid;
	}

	template <typename OUT>
	void encode(OUT& outPacket)
	{
		outPacket<<this->m_nqueue<< this->m_user_mid;
	}


	UMID m_user_mid;
	uint32 m_nqueue;
};

struct CSMsg_Response_User_Depart
{
	CSMsg_Response_User_Depart():m_nqueue(0)
	{
	}
	
	CSMsg_Response_User_Depart(uint32 queue_id):m_nqueue(queue_id)
	{
	}

	template<typename IN>
	void decode(IN& inPacket)
	{
		inPacket >> m_nqueue;
	}

	template<typename OUT>
	void encode(OUT& outPacket)
	{
		outPacket<<m_nqueue;
		/*
		uint16 cnt = m_vec_users.size();
		outPacket << cnt;
		BOOST_FOREACH(USER_INFO& elm,m_vec_users)
		{
			outPacket<< elm.mid.uin <<elm.mid.multptvalue << elm.position;
		}
		*/

	}

	uint32 m_nqueue;
	/*
	std::vector<USER_INFO> m_vec_users;
	*/
};

#endif
