#ifndef CSMSG_AGENT_REFUSE_H_
#define CSMSG_AGENT_REFUSE_H_

#include "type.h"
#include "UMID.h"

struct CSMsg_Request_Agent_Refuse
{
	CSMsg_Request_Agent_Refuse():
		m_nqueue(0)
	{
		memset(&m_agent_mid,0,sizeof(UMID));
	};

	CSMsg_Request_Agent_Refuse(const uint32& queue_id,const UMID& agent):
		m_nqueue(queue_id),m_agent_mid(agent)
	{
	};


	~CSMsg_Request_Agent_Refuse(){};

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


struct CSMsg_Response_Agent_Refuse
{
	CSMsg_Response_Agent_Refuse(const uint32& queue_id):
		m_nqueue(queue_id)
	{
	}


	CSMsg_Response_Agent_Refuse():
		m_nqueue(0)
	{
	}

	template<typename IN>
	void decode(IN& inPacket)
	{
		inPacket>>m_nqueue;
	}

	template<typename OUT>
	void encode(OUT& outPacket)
	{
		outPacket<<m_nqueue;
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
	/*
	std::vector<INVITE_INFO> m_vec_invite;
	*/
};

#endif
