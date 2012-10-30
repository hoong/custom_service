#ifndef CSIMSMSG_USER_NEWCVS_H
#define CSIMSMSG_USER_NEWCVS_H
#include <boost/foreach.hpp>
#include "CSMsg_Datatype.h"
#include "type.h"

struct CSIMSMsg_Request_User_Newcvs 
{
	CSIMSMsg_Request_User_Newcvs():
		m_nqueue(0),m_ncvsid(0),m_agent_name()
	{
		memset(&m_user,0,sizeof(m_user));
	}

	CSIMSMsg_Request_User_Newcvs(const uint32& queue_id,const uint64& cvs_id,const std::string& name):
		m_nqueue(queue_id),m_ncvsid(cvs_id),m_agent_name(name)
	{
		memset(&m_user,0,sizeof(m_user));
	}



	template <typename IN>
	void decode(IN& inPacket)
	{
		inPacket>>this->m_user.uin>>this->m_user.multptvalue
			>>this->m_nqueue 
			>>this->m_ncvsid 
			>>m_agent_name;
	}

	template <typename OUT>
	void encode(OUT& outPacket)
	{
		outPacket <<this->m_user.uin<<this->m_user.multptvalue
			<<this->m_nqueue
			<<this->m_ncvsid 
			<<m_agent_name;
	}

	uint32 m_nqueue;
	uint64 m_ncvsid;
	std::string m_agent_name;
	USER_MULTPT_ID m_user;

};

struct CSIMSMsg_Response_User_Newcvs 
{
	CSIMSMsg_Response_User_Newcvs (uint32 queue_id):m_nqueue(queue_id)
	{
	}

	CSIMSMsg_Response_User_Newcvs ()
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
