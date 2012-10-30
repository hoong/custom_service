#ifndef CSMSG_AGENT_ACCEPT_H_
#define CSMSG_AGENT_ACCEPT_H_

#include "CSMsg_Datatype.h"
#include "type.h"
#include <boost/foreach.hpp>
#include <vector>

struct CSMsg_Request_Agent_Accept
{
	CSMsg_Request_Agent_Accept():m_nqueue(0)
	{
		memset(&m_agent_mid,0,sizeof(UMID));
	};

	CSMsg_Request_Agent_Accept(const uint32& queue_id,const UMID& agent):m_nqueue(queue_id),m_agent_mid(agent)
	{
	};



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

struct CSMsg_Response_Agent_Accept
{
	CSMsg_Response_Agent_Accept():
		m_nqueue(0)
	{}

	CSMsg_Response_Agent_Accept(const uint32& queue_id):
		m_nqueue(queue_id)
	{}


	template <typename IN>
	void decode(IN& inPacket)
	{
		inPacket >>m_nqueue >>m_cvsinfo;
	}

	template <typename OUT>
	void encode(OUT& outPacket)
	{
		//uint16 cnt;
		outPacket <<m_nqueue <<m_cvsinfo;
		/*
			<<m_cvsinfo.conversation_id
			<<m_cvsinfo.agent.uin <<m_cvsinfo.agent.multptvalue <<m_cvsinfo.agent_name 
			<<m_cvsinfo.user.uin <<m_cvsinfo.user.multptvalue <<m_cvsinfo.user_addr;
			*/
		/*
		cnt = m_vec_invite.size();
		outPacket << cnt;

		BOOST_FOREACH(INVITE_INFO& elm,m_vec_invite)
		{
			outPacket<<elm.agent_mid.uin<<elm.agent_mid.multptvalue;
		}

		cnt = m_vec_users.size();
		outPacket << cnt;
		BOOST_FOREACH(USER_INFO& elm,m_vec_users)
		{
			outPacket<< elm.mid.uin <<elm.mid.multptvalue << elm.position;
		}
		*/

	}

	uint32 m_nqueue;
	CONVERSATION_INFO m_cvsinfo;
	/*
	uint64 m_ncvsid;
	uint32 m_naddr;
	*/
	//CONVERSATION_INFO m_cvsinfo;
	/*
	std::vector<INVITE_INFO> m_vec_invite;
	std::vector<USER_INFO> m_vec_users;
	*/
};

#endif
