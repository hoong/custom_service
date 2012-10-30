#ifndef CSMSG_USER_JOIN_H_
#define CSMSG_USER_JOIN_H_
#include "type.h"
#include "CSMsg_Datatype.h"

struct CSMsg_Request_User_Join
{
	CSMsg_Request_User_Join():
		m_nqueue(0),m_naddr(0)
	{
		memset(&m_user_mid,0,sizeof(UMID));
	}

	CSMsg_Request_User_Join(uint32 nQueueid, uint64 nUserUid, uint64 nUserMultpt, uint32 nAddr):
		m_nqueue(nQueueid),m_naddr(nAddr)
	{
		m_user_mid.uin = nUserUid;
		m_user_mid.multptvalue = nUserMultpt;
	}

	template <typename IN>
	void decode(IN& inPacket)
	{
		inPacket >>this->m_nqueue >> this->m_user_mid.uin>>this->m_user_mid.multptvalue>>this->m_naddr;
	}

	template <typename OUT>
	void encode(OUT& outPacket)
	{
		outPacket <<this->m_nqueue << this->m_user_mid.uin<<this->m_user_mid.multptvalue<< this->m_naddr;
	}



	UMID m_user_mid;
	uint32 m_nqueue;
	uint32 m_naddr;
};

struct CSMsg_Response_User_Join
{
	CSMsg_Response_User_Join():m_nqueue(0),m_npos(0)
	{
	}
	
	CSMsg_Response_User_Join(uint32 queue_id):m_nqueue(queue_id),m_npos(0)
	{
	}

	template <typename IN>
	void decode(IN& inPacket)
	{
		inPacket >> m_nqueue >> m_npos ;
	}

	template <typename OUT>
	void encode(OUT& outPacket)
	{

		outPacket << m_nqueue <<m_npos ;
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
	uint16 m_npos;
	// std::vector<INVITE_INFO> m_vec_invite;


};

#endif
