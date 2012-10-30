#ifndef CUSTOM_SERVICE_QUEUEMGR_H_
#define CUSTOM_SERVICE_QUEUEMGR_H_

#include "packet/header.h"
#include "Queue.h"
#include "CSMySqlDbMgr.h"
#include "CSNetHandler.h"

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
#include "CSMsg_Get_Msg.h"
#include "CSMsg_Send_Msg.h"

#include <boost/thread/detail/singleton.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>


namespace custom_service
{

class CSQueueMgr
{
public:
	bool initialize(const char* conf_file);



	//Return a Bool value to determin whether to send_packet; return true to send.
	void processRequest(base::packet::Header& header,CSMsg_Request_Agent_Presence& request,const boost::shared_ptr<base::NetHandlerBase>& spHandler);
	void processRequest(base::packet::Header& header,CSMsg_Request_Agent_Absence& request,const boost::shared_ptr<base::NetHandlerBase>& spHandler);
	void processRequest(base::packet::Header& header,CSMsg_Request_Agent_Redirect& request,const boost::shared_ptr<base::NetHandlerBase>& spHandler);
	void processRequest(base::packet::Header& header,CSMsg_Request_Agent_Get_Queue& request,const boost::shared_ptr<base::NetHandlerBase>& spHandler);
	void processRequest(base::packet::Header& header,CSMsg_Request_Agent_Get_QueueAgent& request,const boost::shared_ptr<base::NetHandlerBase>& spHandler);
	void processRequest(base::packet::Header& header,CSMsg_Request_Agent_Accept& request,const boost::shared_ptr<base::NetHandlerBase>& spHandler);
	void processRequest(base::packet::Header& header,CSMsg_Request_Agent_Refuse& request,const boost::shared_ptr<base::NetHandlerBase>& spHandler);
	void processRequest(base::packet::Header& header,CSMsg_Request_Agent_Useramount& request,const boost::shared_ptr<base::NetHandlerBase>& spHandler);
	void processRequest(base::packet::Header& header,CSMsg_Request_Agent_Offline& request,const boost::shared_ptr<base::NetHandlerBase>& spHandler);

	void processRequest(base::packet::Header& header,CSMsg_Request_User_Score& request,const boost::shared_ptr<base::NetHandlerBase>& spHandler);
	void processRequest(base::packet::Header& header,CSMsg_Request_User_Join& request,const boost::shared_ptr<base::NetHandlerBase>& spHandler);
	void processRequest(base::packet::Header& header,CSMsg_Request_User_Depart& request,const boost::shared_ptr<base::NetHandlerBase>& spHandler);

	void processRequest(base::packet::Header& header,CSMsg_Request_Conversation_End& request,const boost::shared_ptr<base::NetHandlerBase>& spHandler);
	void processRequest(base::packet::Header& header,CSMsg_Request_Send_Msg& request,const boost::shared_ptr<base::NetHandlerBase>& spHandler);
	void processRequest(base::packet::Header& header,CSMsg_Request_Get_Msg& request,const boost::shared_ptr<base::NetHandlerBase>& spHandler);

	void test();

private:
	QUEUE* find(const uint32& queue_id);
	QUEUE* insert(const uint32& queue_id);

private:
	boost::mutex m_pool_mutex;
	std::map<uint32,QUEUE> m_queue_pool;

	//boost::scoped_ptr<IDbMgr*> m_sp_dbmgr;
	boost::shared_ptr<IDbMgr> m_sp_dbmgr;
};


typedef boost::detail::thread::singleton<CSQueueMgr> CSMGR;

}

#endif
