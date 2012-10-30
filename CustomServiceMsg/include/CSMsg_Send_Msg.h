#ifndef CSMSG_SEND_MSG_H_
#define CSMSG_SEND_MSG_H_

#include <boost/foreach.hpp>
#include "CSMsg_Datatype.h"
#include <string>


struct CSMsg_Request_Send_Msg
{
	CSMsg_Request_Send_Msg():
		m_nqueue(0),m_ncvsid(0)
	{
	}

	CSMsg_Request_Send_Msg(const uint32& queue_id,const uint64& cvs_id,const UMID& sender,const std::string& text):
		m_nqueue(queue_id),m_ncvsid(cvs_id),m_sender(sender),m_text(text)
	{
	}


	template <typename IN>
	void decode(IN& inPacket)
	{
		inPacket>> this->m_nqueue >>this->m_ncvsid>>this->m_sender>>this->m_text;
	}

	template <typename OUT>
	void encode(OUT& outPacket)
	{
		outPacket<< this->m_nqueue <<this->m_ncvsid<<this->m_sender<<this->m_text;
	}


	uint32 m_nqueue;
	uint64 m_ncvsid;
	UMID m_sender;
	std::string m_text;
};

struct CSMsg_Response_Send_Msg
{
	CSMsg_Response_Send_Msg(const uint64& queue_id):
		m_nqueue(queue_id)
	{
	}

	CSMsg_Response_Send_Msg()
	{
	}

	template <typename IN>
	void decode(IN& inPacket)
	{
		inPacket>>m_nqueue>>m_receiver;
	}

	template <typename OUT>
	void encode(OUT& outPacket)
	{
		outPacket<<m_nqueue<<m_receiver;
	}


	uint32 m_nqueue;
	UMID m_receiver;
};

#endif
