#ifndef CUSTOM_SERVICE_NET_HANDLER_H_
#define CUSTOM_SERVICE_NET_HANDLER_H_

#include "type.h"
#include "net/handler.h"
#include "packet/out_packet.h"
#include "packet/in_packet.h"
#include "packet/header.h"
#include "logger.h"
//#include "command.h"
#include "cmd.h"
#include "CSThreadPool.h"
#include "CSProcessTask.h"
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
//packet
#include "packet/in_packet.h"
#include "packet/out_packet.h"
#include "packet/header.h" 
namespace cs_client {


class CSNetHandler:public base::NetHandler<CSNetHandler,base::packet::Header>
{

public:
	virtual int open();
	virtual void handle_close();
	virtual int handle_packet(base::packet::Header& header, base::packet::InPacket& in);

private:
	template <typename RESPONSE,typename BSN_MGR>
	void processBSNResponse(RESPONSE& res,BSN_MGR& bsnmgr,base::packet::Header& header,base::packet::InPacket& in);
	void processTask(base::packet::Header& header, boost::shared_ptr<base::packet::InPacket>& in);
	int sendErrCode(uint16 cmd,uint32 seq,uint16 err);

};

}

#endif
