#ifndef CSMSG_DATATYPE_H_
#define CSMSG_DATATYPE_H_

#include "type.h"
//#include "UserMultptID.h"
#include "UMID.h"
#include "packet/out_packet.h"
#include <string>


enum 
{
	RET_LIST_NEW_CONVERSATION = 1,
	RET_LIST_END_CONVERSATION,
	RET_LIST_USER_STATUS,
	RET_LIST_AGENT_ENQUIRE
};

struct CONVERSATION_DATA
{
	uint64 conversation_id;
	uint32 queue_id;
	uint64 agent_uid;
	uint64 agent_multpt;
	uint64 user_uid;
	uint64 user_multpt;
};

struct USER_DATA
{
	uint32 queue_id;
	uint64 conversation_id;
	uint64 uid;
	uint64 multpt;
	uint32 position;
};

struct AGENT_DATA
{
//	uint32 queue_id;
	uint64 uid;
	uint64 multpt;
	std::string name;
};


struct QUEUE_DATA
{
	uint32 wg_id;
	std::string wg_name;
	uint32 queue_id;
	std::string queue_name;
};




/*
struct UMID
{
	uint64 uid;
	uint64 multpt;
	inline bool operator ==(const UMID& lv) const
	{
		return (uid == lv.uid && multpt == lv.multpt);
	}
};
*/

struct USER_INFO
{
	UMID mid;
//	uint32 queue_id;
	uint32 position;
};

struct AGENT_INFO
{
	UMID mid;
//	uint32 queue_id;
	std::string name;

	friend base::packet::OutPacket&
	operator << (base::packet::OutPacket& out,const AGENT_INFO& ai)
	{
		out <<ai.mid.uin <<ai.mid.multptvalue <<ai.name;

		return out;
	};
};

/*
struct INVITE_INFO
{
	UMID agent_mid;
	uint32 queue_id;
};
*/
typedef UMID INVITE_INFO;


struct CONVERSATION_INFO
{
	uint64 conversation_id;
	UMID agent;
	std::string agent_name;
	UMID user;
	uint32 user_addr;

	friend base::packet::OutPacket&
	operator << (base::packet::OutPacket& out,const CONVERSATION_INFO& ci)
	{
		out <<ci.conversation_id <<ci.agent <<ci.agent_name <<ci.user <<ci.user_addr;
		return out;
	};

	friend base::packet::InPacket&
	operator >> (base::packet::InPacket& in,CONVERSATION_INFO& ci)
	{
		in >>ci.conversation_id >>ci.agent >>ci.agent_name >>ci.user >>ci.user_addr;
		return in;
	};

};

struct USER_CONVERSATION
{
	//USER_INFO user;
	UMID mid;
	uint64 conversation_id;
	//uint32 queue_id;
};

struct MSG_INFO
{
	uint64 msg_id;
	uint64 cvs_id;
	uint64 agent;
	uint64 user;
	uint8 send_flag;
	time_t send_time;
	std::string send_text;

	friend base::packet::OutPacket&
	operator << (base::packet::OutPacket& out,const MSG_INFO& mi)
	{
		out << mi.msg_id <<mi.cvs_id <<mi.agent <<mi.user <<mi.send_flag <<mi.send_time <<mi.send_text;
		return out;
	};

	friend base::packet::InPacket&
	operator >> (base::packet::InPacket& in,MSG_INFO& mi)
	{
		in >> mi.msg_id >>mi.cvs_id >>mi.agent >>mi.user >>mi.send_flag >>mi.send_time >>mi.send_text;
		return in;
	};



};



struct QUEUE_INFO
{
	uint32 wg_id;
	std::string wg_name;
	uint32 queue_id;
	std::string queue_name;

	friend base::packet::OutPacket&
	operator << (base::packet::OutPacket& out,const QUEUE_INFO& qi)
	{
		out <<qi.wg_id <<qi.wg_name <<qi.queue_id <<qi.queue_name;

		return out;
	};
};

struct ICQ_DATA:public base::packet::OutPacket
{
	friend base::packet::OutPacket&
	operator <<(base::packet::OutPacket& out,ICQ_DATA& i)
	{
		out <<(uint16) i.get_size();
		out.write_data(i.m_evbuf,0);
		return out;
	};
};


/*
struct AGENT_CONVERSATION
{
	AGENT_INFO agent;
	uint64 conversation_id;

};
*/




#endif
