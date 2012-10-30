#ifndef CSIMSMSG_RELAY_MSG_H
#define CSIMSMSG_RELAY_MSG_H
#include <boost/foreach.hpp>
#include "CSMsg_Datatype.h"
#include "type.h"

struct CSIMSMsg_Request_Relay_Msg 
{
	CSIMSMsg_Request_Relay_Msg():
		m_ncvsid(0)
	{
		memset(&m_recv,0,sizeof(m_recv));
	}

/*
	CSIMSMsg_Request_Relay_Msg(const uint32& queue_id,const uint64& cvs_id,const std::string& name):
		m_nqueue(queue_id),m_ncvsid(cvs_id),m_agent_name(name)
	{
		memset(&m_user,0,sizeof(m_user));
	}
*/



	template <typename IN>
	void decode(IN& inPacket)
	{
		inPacket>>this->m_ncvsid
			>>this->m_recv.uin>>this->m_recv.multptvalue
			>>this->m_text;
	}

	template <typename OUT>
	void encode(OUT& outPacket)
	{
		outPacket<<this->m_ncvsid
			<<this->m_recv.uin<<this->m_recv.multptvalue
			<<this->m_text;

	}

	uint64 m_ncvsid;
	USER_MULTPT_ID m_recv;
	std::string m_text;

};

struct CSIMSMsg_Response_Relay_Msg 
{
	/*
	CSIMSMsg_Response_Relay_Msg (uint32 queue_id):m_nqueue(queue_id)
	{
	}
	*/

	CSIMSMsg_Response_Relay_Msg ()
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


};

#endif
