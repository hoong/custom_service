#ifndef CSIMSMSG_AGENT_DELCVS_H
#define CSIMSMSG_AGENT_DELCVS_H
#include "CSMsg_Datatype.h"
#include "type.h"
#include <boost/foreach.hpp>

struct CSIMSMsg_Request_Agent_Delcvs 
{
	CSIMSMsg_Request_Agent_Delcvs():
		m_nqueue(0),m_ncvsid(0)
	{
		memset(&m_agent,0,sizeof(m_agent));
	}
	CSIMSMsg_Request_Agent_Delcvs(const uint32& queue_id,const uint64& cvs_id):
		m_nqueue(queue_id),m_ncvsid(cvs_id)
	{
		memset(&m_agent,0,sizeof(m_agent));
	}



	template <typename IN>
	void decode(IN& inPacket)
	{
		inPacket>>this->m_agent.uin>>this->m_agent.multptvalue>>this->m_nqueue >>this->m_ncvsid ;
	}

	template <typename OUT>
	void encode(OUT& outPacket)
	{
		outPacket<<this->m_agent.uin<<this->m_agent.multptvalue<<this->m_nqueue<<this->m_ncvsid ;
	}

	uint32 m_nqueue;
	uint64 m_ncvsid;
	USER_MULTPT_ID m_agent;

};

struct CSIMSMsg_Response_Agent_Delcvs 
{
	CSIMSMsg_Response_Agent_Delcvs (uint32 queue_id):m_nqueue(queue_id)
	{
	}

	CSIMSMsg_Response_Agent_Delcvs ()
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
