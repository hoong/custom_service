/*
 * OMSClient.h
 *
 *  Created on: 2012-2-19
 *      Author: root
 */

#ifndef CS_PROCESS_TASK_H_
#define CS_PROCESS_TASK_H_

#include <boost/thread/detail/singleton.hpp>
#include "CallbackQueue.h"
//#include "command.h"
#include "cmd.h"
//msg define
#include "CSMsg_Datatype.h"
#include "CSMsg_Agent_Absence.h"
#include "CSMsg_Agent_Accept.h"
#include "CSMsg_Agent_Get_QueueAgent.h"
#include "CSMsg_Agent_Get_Queue.h"
#include "CSMsg_Agent_Offline.h"
#include "CSMsg_Agent_Presence.h"
#include "CSMsg_Agent_Redirect.h"
#include "CSMsg_Agent_Refuse.h"
#include "CSMsg_Agent_Useramount.h"
#include "CSMsg_Conversation_End.h"
#include "CSMsg_User_Depart.h"
#include "CSMsg_User_Join.h"
#include "CSMsg_User_Score.h"
#include "CSMsg_Send_Msg.h"
#include "CSMsg_Get_Msg.h"
//packet
#include "packet/in_packet.h"
#include "packet/out_packet.h"
#include "packet/header.h"


namespace cs_client
{
class CSProcessTask
{
public:
	CSProcessTask()
	{

	}
	virtual ~CSProcessTask()
	{

	}
		
public:
	//void processResponse(base::packet::Header& header,base::packet::InPacket& in);
	void processResponse(base::packet::Header& header,CSMsg_Response_User_Join& res);
	void processResponse(base::packet::Header& header,CSMsg_Response_User_Depart& res);
	void processResponse(base::packet::Header& header,CSMsg_Response_User_Score& res);

		
	void processResponse(base::packet::Header& header,base::packet::OutPacket* out,CSMsg_Response_Agent_Presence& res);
	void processResponse(base::packet::Header& header,base::packet::OutPacket* out,CSMsg_Response_Agent_Absence& res);
	void processResponse(base::packet::Header& header,base::packet::OutPacket* out,CSMsg_Response_Agent_Redirect& res);
	void processResponse(base::packet::Header& header,base::packet::OutPacket* out,CSMsg_Response_Agent_Get_Queue& res);
	void processResponse(base::packet::Header& header,base::packet::OutPacket* out,CSMsg_Response_Agent_Get_QueueAgent& res);
	void processResponse(base::packet::Header& header,base::packet::OutPacket* out,CSMsg_Response_Agent_Accept& res);
	void processResponse(base::packet::Header& header,base::packet::OutPacket* out,CSMsg_Response_Agent_Refuse& res);
	void processResponse(base::packet::Header& header,base::packet::OutPacket* out,CSMsg_Response_Agent_Useramount& res);
	void processResponse(base::packet::Header& header,base::packet::OutPacket* out,CSMsg_Response_User_Join& res);
	void processResponse(base::packet::Header& header,base::packet::OutPacket* out,CSMsg_Response_User_Score& res);
	void processResponse(base::packet::Header& header,base::packet::OutPacket* out,CSMsg_Response_Conversation_End& res);
	void processResponse(base::packet::Header& header,base::packet::OutPacket* out,CSMsg_Response_User_Depart& res);
	void processResponse(base::packet::Header& header,base::packet::OutPacket* out,CSMsg_Response_Agent_Offline& res);

	void processResponse(base::packet::Header& header,base::packet::OutPacket* out,CSMsg_Response_Send_Msg& res);
	void processResponse(base::packet::Header& header,base::packet::OutPacket* out,CSMsg_Response_Get_Msg& res);
};

	typedef boost::detail::thread::singleton<CSProcessTask> CS_PRO_TASK;
}
#endif

