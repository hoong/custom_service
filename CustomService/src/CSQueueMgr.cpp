#include <stdio.h>
#include "CSQueueMgr.h"
#include "CSImsMgr.h"
#include "logger.h"
#include <boost/foreach.hpp>
#include "type.h"
#include "Sequence.h"

namespace custom_service
{

void dispatch_invite_list(const uint32& queue_id,const std::vector<INVITE_INFO>& invite_list)
{
	if (invite_list.size() == 0)
	{
		return;
	}

	listmap<INVITE_INFO> lm;
	for (size_t idx =0;idx<invite_list.size();idx++)
	{
		lm.insert(invite_list[idx].multptinfo.imsid,invite_list[idx]);
	}

	uint16 imsid;
	std::vector<INVITE_INFO>* ii_ptr;
	while(lm.next(imsid,ii_ptr))
	{
		boost::shared_ptr<CSImsHandler> spHandler;
		CSIMSMGR::instance().get_handler(imsid,spHandler);
		if(spHandler.get())
		{
			spHandler->send_invite_list(queue_id,*ii_ptr);
		};
	}

};

void dispatch_usercvs_list(const uint32& queue_id,const std::vector<USER_CONVERSATION>& usercvs_list)
{
	if(usercvs_list.size() == 0)
	{
		return ;
	}

	listmap<USER_CONVERSATION> lm;
	for (size_t idx = 0;idx <usercvs_list.size();idx++)
	{
		lm.insert(usercvs_list[idx].mid.multptinfo.imsid,usercvs_list[idx]);
	}

	uint16 imsid;
	std::vector<USER_CONVERSATION>* uc_ptr;
	while(lm.next(imsid,uc_ptr))
	{
		boost::shared_ptr<CSImsHandler> spHandler;
		CSIMSMGR::instance().get_handler(imsid,spHandler);
		if(spHandler.get())
		{
			spHandler->send_user_delcvs(queue_id,*uc_ptr);
		};

	};
	 return;
};

void dispatch_user_list(const uint32& queue_id,std::vector<USER_INFO>& user_list)
{
	if(user_list.size() == 0)
	{
		return;
	}

	listmap<USER_INFO> lm;
	for (size_t idx = 0;idx<user_list.size();idx++)
	{
		lm.insert(user_list[idx].mid.multptinfo.imsid,user_list[idx]);
	}

	uint16 imsid;
	std::vector<USER_INFO>* ui_ptr;
	while(lm.next(imsid,ui_ptr))
	{
		boost::shared_ptr<CSImsHandler> spHandler;
		CSIMSMGR::instance().get_handler(imsid,spHandler);
		if(spHandler.get())
		{
			spHandler->send_user_list(queue_id,*ui_ptr);
		};

	};



};


bool CSQueueMgr::initialize(const char* conf_file)
{
	//m_sp_dbmgr.reset((IDbMgr*)new CMySqlDbMgr());
	m_sp_dbmgr.reset(new CMySqlDbMgr());
	bool ret = m_sp_dbmgr->init();

	if (ret)
	{
		uint64 id_init = 0;
		m_sp_dbmgr->last_cvsid(id_init);
		SEQMGR::instance().init_cvs(id_init);
		m_sp_dbmgr->last_msgid(id_init);
		SEQMGR::instance().init_msg(id_init);
	}

	//test();
	return ret;

	//return m_db.initialize(conf_file);
}

QUEUE* CSQueueMgr::find(const uint32& queue_id)
{
	std::map<uint32,QUEUE>::iterator it = m_queue_pool.find(queue_id);
	if (it == m_queue_pool.end())
	{
		return NULL;
	}
	QUEUE* queue_ptr = &(it->second);
	return queue_ptr;
}

QUEUE* CSQueueMgr::insert(const uint32& queue_id)
{
	boost::mutex::scoped_lock lock(m_pool_mutex);

	//QUEUE queue(&m_db);
	QUEUE queue(m_sp_dbmgr.get());
	if (queue.load(queue_id))
	{
		std::pair<std::map<uint32,QUEUE>::iterator,bool> ret = m_queue_pool.insert(std::pair<uint32,QUEUE>(queue_id,queue));
		QUEUE* queue_ptr = &(ret.first->second);
		return queue_ptr;

	}

	return NULL;
}


void CSQueueMgr::processRequest(base::packet::Header& header,
		CSMsg_Request_Agent_Presence& request,
		const boost::shared_ptr<base::NetHandlerBase>& spHandler)
{
	base::packet::OutPacket out;
	QUEUE* queue_ptr;
	CSMsg_Response_Agent_Presence response(request.m_nqueue);

	std::vector<INVITE_INFO> invite_list;

	if  ( (queue_ptr = this->find(request.m_nqueue)) ||( queue_ptr = this->insert(request.m_nqueue)))
	{
		if (queue_ptr->agent_presence(request.m_agent_mid,request.m_ncapacity,invite_list))
		{
			header.status_code(200);
		}
		else 
		{
			header.status_code(400);
		}
		dispatch_invite_list(queue_ptr->queue_id,invite_list);
	}
	else
	{
		LOG(error)<<"Queue_id = "<<request.m_nqueue<<" not exists!"<<ENDL;
		header.status_code(500);
	}

	response.encode(out);
	boost::shared_ptr<CSNetHandler> handler = boost::dynamic_pointer_cast<CSNetHandler>(spHandler);
	handler->send_packet(header,out);

}

void CSQueueMgr::processRequest(base::packet::Header& header,
		CSMsg_Request_Agent_Absence& request,
		const boost::shared_ptr<base::NetHandlerBase>& spHandler)
{
	base::packet::OutPacket out;
	CSMsg_Response_Agent_Absence response(request.m_nqueue);
	
	QUEUE* queue_ptr;
	if  (( queue_ptr = find(request.m_nqueue)) || (queue_ptr = insert(request.m_nqueue)))
	{
		if (queue_ptr->agent_absence(request.m_agent_mid))
		{
			header.status_code(200);
		}
		else 
		{
			header.status_code(400);
		}
	}
	else
	{
		LOG(error)<<"Queue_id = "<<request.m_nqueue<<" not exists!"<<ENDL;
		header.status_code(500);
	}

	response.encode(out);
	boost::shared_ptr<CSNetHandler> handler = boost::dynamic_pointer_cast<CSNetHandler>(spHandler);
	handler->send_packet(header,out);

}

void CSQueueMgr::processRequest(base::packet::Header& header,
		CSMsg_Request_Agent_Offline& request,
		const boost::shared_ptr<base::NetHandlerBase>& spHandler)
{
	base::packet::OutPacket out;
	CSMsg_Response_Agent_Offline response(request.m_nqueue);
	
	std::vector<USER_CONVERSATION> usercvs_list;
	std::vector<INVITE_INFO> invite_list;
	QUEUE* queue_ptr;
	if  (( queue_ptr = find(request.m_nqueue)) ||( queue_ptr = insert(request.m_nqueue)))
	{
		if (queue_ptr->agent_offline(request.m_agent_mid,usercvs_list,invite_list))
		{
			header.status_code(200);
		}
		else 
		{
			header.status_code(400);
		}
		dispatch_invite_list(queue_ptr->queue_id,invite_list);
		dispatch_usercvs_list(queue_ptr->queue_id,usercvs_list);
	}
	else
	{
		LOG(error)<<"Queue_id = "<<request.m_nqueue<<" not exists!"<<ENDL;
		header.status_code(500);
	}

	response.encode(out);
	boost::shared_ptr<CSNetHandler> handler = boost::dynamic_pointer_cast<CSNetHandler>(spHandler);
	handler->send_packet(header,out);

}

void CSQueueMgr::processRequest(base::packet::Header& header,
		CSMsg_Request_Agent_Redirect& request,
		const boost::shared_ptr<base::NetHandlerBase>& spHandler)
{
	base::packet::OutPacket out;

	CSMsg_Response_Agent_Redirect response(request.m_nqueue);

	QUEUE* queue_ptr;
	std::vector<INVITE_INFO> invite_list;
	if  (( queue_ptr = find(request.m_nqueue)) || (queue_ptr = insert(request.m_nqueue)))
	{
		if (queue_ptr->agent_redirect(request.m_ncvsid,request.m_agent_from,request.m_agent_to,invite_list))
		{
			header.status_code(200);
		}
		else 
		{
			header.status_code(400);
		}
		dispatch_invite_list(queue_ptr->queue_id,invite_list);
	}
	else
	{
		LOG(error)<<"Queue_id = "<<request.m_nqueue<<" not exists!"<<ENDL;
		header.status_code(500);
	}

	response.encode(out);

	boost::shared_ptr<CSNetHandler> handler = boost::dynamic_pointer_cast<CSNetHandler>(spHandler);
	handler->send_packet(header,out);

}

void CSQueueMgr::processRequest(base::packet::Header& header,
		CSMsg_Request_Agent_Get_Queue& request,
		const boost::shared_ptr<base::NetHandlerBase>& spHandler)
{
	base::packet::OutPacket out;
	CSMsg_Response_Agent_Get_Queue response(request.m_nuid);
	if (m_sp_dbmgr->get_queue_list(request.m_nuid,response.m_queue_list))
	{
		header.status_code(200);
	}
	else
	{
		header.status_code(500);
	}

	response.encode(out);
	boost::shared_ptr<CSNetHandler> handler = boost::dynamic_pointer_cast<CSNetHandler>(spHandler);
	handler->send_packet(header,out);

}


void CSQueueMgr::processRequest(base::packet::Header& header,
		CSMsg_Request_Agent_Get_QueueAgent& request,
		const boost::shared_ptr<base::NetHandlerBase>& spHandler)
{
	base::packet::OutPacket out;
	CSMsg_Response_Agent_Get_QueueAgent response(request.m_nqueue);

	QUEUE* queue_ptr;
	if  (( queue_ptr = find(request.m_nqueue)) || (queue_ptr = insert(request.m_nqueue)))
	{
		if (queue_ptr->agent_list(response.m_agent_list))
		{
			header.status_code(200);
		}
		else 
		{
			header.status_code(400);
		}
	}
	else
	{
		LOG(error)<<"Queue_id = "<<request.m_nqueue<<" not exists!"<<ENDL;
		header.status_code(500);
	}

	response.encode(out);
	boost::shared_ptr<CSNetHandler> handler = boost::dynamic_pointer_cast<CSNetHandler>(spHandler);
	handler->send_packet(header,out);

}

void CSQueueMgr::processRequest(base::packet::Header& header,
		CSMsg_Request_Agent_Accept& request,
		const boost::shared_ptr<base::NetHandlerBase>& spHandler)
{
	base::packet::OutPacket out;
	CSMsg_Response_Agent_Accept response(request.m_nqueue);

	QUEUE* queue_ptr;
	std::vector<INVITE_INFO> invite_list;
	std::vector<USER_INFO> user_list;
	CONVERSATION_INFO cvs_info;
	if  (( queue_ptr = find(request.m_nqueue)) || (queue_ptr = insert(request.m_nqueue)))
	{
		if (queue_ptr->agent_accept(request.m_agent_mid,cvs_info,invite_list,user_list))
		{
			header.status_code(200);
			response.m_cvsinfo = cvs_info;

			/*
			boost::shared_ptr<CSImsHandler> spHandler;
			CSIMSMGR::instance().get_handler(cvs_info.user.multptinfo.imsid,spHandler);
			if(spHandler.get())
			{
				spHandler->send_user_newcvs(queue_ptr->queue_id,cvs_info.conversation_id,cvs_info.agent_name,cvs_info.user);
			};
			*/
		}
		else 
		{
			header.status_code(400);
		}
		dispatch_invite_list(queue_ptr->queue_id,invite_list);
		dispatch_user_list(queue_ptr->queue_id,user_list);
	}
	else
	{
		LOG(error)<<"Queue_id = "<<request.m_nqueue<<" not exists!"<<ENDL;
		header.status_code(500);
	}
	response.encode(out);
	boost::shared_ptr<CSNetHandler> handler = boost::dynamic_pointer_cast<CSNetHandler>(spHandler);
	handler->send_packet(header,out);

}


void CSQueueMgr::processRequest(base::packet::Header& header,
		CSMsg_Request_Agent_Refuse& request,
		const boost::shared_ptr<base::NetHandlerBase>& spHandler)
{
	base::packet::OutPacket out;
	CSMsg_Response_Agent_Refuse response(request.m_nqueue);

	QUEUE* queue_ptr;
	std::vector<INVITE_INFO> invite_list;
	if  (( queue_ptr = find(request.m_nqueue)) || (queue_ptr = insert(request.m_nqueue)))
	{
		if (queue_ptr->agent_refuse(request.m_agent_mid,invite_list))
		{
			header.status_code(200);
		}
		else 
		{
			header.status_code(400);
		}
		dispatch_invite_list(queue_ptr->queue_id,invite_list);
	}
	else
	{
		LOG(error)<<"Queue_id = "<<request.m_nqueue<<" not exists!"<<ENDL;
		header.status_code(500);
	}

	response.encode(out);
	boost::shared_ptr<CSNetHandler> handler = boost::dynamic_pointer_cast<CSNetHandler>(spHandler);
	handler->send_packet(header,out);

}


void CSQueueMgr::processRequest(base::packet::Header& header,
		CSMsg_Request_Agent_Useramount& request,
		const boost::shared_ptr<base::NetHandlerBase>& spHandler)
{
	base::packet::OutPacket out;
	CSMsg_Response_Agent_Useramount response(request.m_nqueue);

	QUEUE* queue_ptr;
	if  (( queue_ptr = find(request.m_nqueue)) || (queue_ptr = insert(request.m_nqueue)))
	{
		response.m_namount = queue_ptr->user_amount();
		header.status_code(200);

	}
	else
	{
		LOG(error)<<"Queue_id = "<<request.m_nqueue<<" not exists!"<<ENDL;
		header.status_code(500);
	}

	response.encode(out);
	boost::shared_ptr<CSNetHandler> handler = boost::dynamic_pointer_cast<CSNetHandler>(spHandler);
	handler->send_packet(header,out);

}


void CSQueueMgr::processRequest(base::packet::Header& header,
		CSMsg_Request_User_Score& request,
		const boost::shared_ptr<base::NetHandlerBase>& spHandler)
{
	base::packet::OutPacket out;
	CSMsg_Response_User_Score response(request.m_nqueue);

	QUEUE* queue_ptr;
	if  (( queue_ptr = find(request.m_nqueue)) || (queue_ptr = insert(request.m_nqueue)))
	{
		if (queue_ptr->user_score(request.m_user_mid,request.m_ncvsid,request.m_nscore))
		{
			header.status_code(200);
		}
		else 
		{
			header.status_code(400);
		}
	}
	else
	{
		LOG(error)<<"Queue_id = "<<request.m_nqueue<<" not exists!"<<ENDL;
		header.status_code(500);
	}

	response.encode(out);
	boost::shared_ptr<CSNetHandler> handler = boost::dynamic_pointer_cast<CSNetHandler>(spHandler);
	handler->send_packet(header,out);

}

void CSQueueMgr::processRequest(base::packet::Header& header,
		CSMsg_Request_User_Join& request,
		const boost::shared_ptr<base::NetHandlerBase>& spHandler)
{
	base::packet::OutPacket out;
	CSMsg_Response_User_Join response(request.m_nqueue);

	QUEUE* queue_ptr;
	std::vector<INVITE_INFO> invite_list;
	if  (( queue_ptr = find(request.m_nqueue)) || (queue_ptr = insert(request.m_nqueue)))
	{
		response.m_npos = queue_ptr->user_join(request.m_user_mid,request.m_naddr,invite_list);
		header.status_code(200);
		dispatch_invite_list(queue_ptr->queue_id,invite_list);
	}
	else
	{
		LOG(error)<<"Queue_id = "<<request.m_nqueue<<" not exists!"<<ENDL;
		header.status_code(500);
	}

	response.encode(out);
	boost::shared_ptr<CSNetHandler> handler = boost::dynamic_pointer_cast<CSNetHandler>(spHandler);
	handler->send_packet(header,out);

}

void CSQueueMgr::processRequest(base::packet::Header& header,
		CSMsg_Request_User_Depart& request,
		const boost::shared_ptr<base::NetHandlerBase>& spHandler)
{
	base::packet::OutPacket out;
	CSMsg_Response_User_Depart response(request.m_nqueue);

	QUEUE* queue_ptr;
	std::vector<USER_INFO> user_list;
	if  (( queue_ptr = find(request.m_nqueue)) || (queue_ptr = insert(request.m_nqueue)))
	{
		if (queue_ptr->user_depart(request.m_user_mid,user_list))
		{
			header.status_code(200);
		}
		else 
		{
			header.status_code(400);
		}
		dispatch_user_list(queue_ptr->queue_id,user_list);
	}
	else
	{
		LOG(error)<<"Queue_id = "<<request.m_nqueue<<" not exists!"<<ENDL;
		header.status_code(500);
	}

	response.encode(out);
	boost::shared_ptr<CSNetHandler> handler = boost::dynamic_pointer_cast<CSNetHandler>(spHandler);
	handler->send_packet(header,out);

}

void CSQueueMgr::processRequest(base::packet::Header& header,
		CSMsg_Request_Conversation_End& request,
		const boost::shared_ptr<base::NetHandlerBase>& spHandler)
{
	base::packet::OutPacket out;
	CSMsg_Response_Conversation_End response(request.m_nqueue);

	QUEUE* queue_ptr;
	std::vector<INVITE_INFO> invite_list;
	UMID agent;
	USER_CONVERSATION user_cvs;
	if  (( queue_ptr = find(request.m_nqueue)) || (queue_ptr = insert(request.m_nqueue)))
	{
		if (queue_ptr->conversation_end(request.m_ncvsid,agent,user_cvs,invite_list))
		{
			header.status_code(200);

			boost::shared_ptr<CSImsHandler> spHandler;
			CSIMSMGR::instance().get_handler(agent.multptinfo.imsid,spHandler);
			if(spHandler.get())
			{
				spHandler->send_agent_delcvs(queue_ptr->queue_id,request.m_ncvsid,agent);
			};
		}
		else 
		{
			header.status_code(400);
		}
		dispatch_invite_list(queue_ptr->queue_id,invite_list);
	}
	else
	{
		LOG(error)<<"Queue_id = "<<request.m_nqueue<<" not exists!"<<ENDL;
		header.status_code(500);
	}

	response.encode(out);
	boost::shared_ptr<CSNetHandler> handler = boost::dynamic_pointer_cast<CSNetHandler>(spHandler);
	handler->send_packet(header,out);

}


void CSQueueMgr::processRequest(base::packet::Header& header,
		CSMsg_Request_Send_Msg& request,
		const boost::shared_ptr<base::NetHandlerBase>& spHandler)
{
	base::packet::OutPacket out;
	CSMsg_Response_Send_Msg response(request.m_nqueue);

	QUEUE* queue_ptr;
	UMID agent;
	USER_CONVERSATION user_cvs;
	if  (( queue_ptr = find(request.m_nqueue)) || (queue_ptr = insert(request.m_nqueue)))
	{

		CONVERSATION* cvs_ptr = queue_ptr->get_cvs(request.m_ncvsid);

		if (cvs_ptr == NULL)
		{
			LOG(error)<<"cvs doesn't exist:cvs_id = "<<request.m_ncvsid<<ENDL;
			header.status_code(400);
		}
		else
		{
			MSG_INFO mi;
			mi.msg_id = SEQMGR::instance().next_msg();
			mi.cvs_id = request.m_ncvsid;
			mi.agent = cvs_ptr->agent->id.uin;
			mi.user = cvs_ptr->user.id.uin;
			mi.send_flag =( cvs_ptr->agent->id == request.m_sender)?0:1;
			mi.send_time= time(NULL);
			mi.send_text  = request.m_text;

			if(m_sp_dbmgr->save_msg(mi))
			{
				response.m_receiver= (mi.send_flag ==0)?cvs_ptr->user.id:cvs_ptr->agent->id;
				header.status_code(200);
			}
			else
			{
				header.status_code(500);
			}

			/*
			uint16 imsid = recv.multptinfo.imsid;

			boost::shared_ptr<CSImsHandler> imsHandler;
			CSIMSMGR::instance().get_handler(imsid,imsHandler);
			if(spHandler.get())
			{
				IMS_CONTEXT_TYPE ic(new relaymsg_context());
				boost::shared_ptr<relaymsg_context> rc = boost::dynamic_pointer_cast<relaymsg_context>(ic); 
				rc->header = header;
				rc->queue_ptr = queue_ptr;
				rc->handler = boost::dynamic_pointer_cast<CSNetHandler>(spHandler);
				rc->dbmgr = m_sp_dbmgr;
				rc->mi = mi;
				//imsHandler->send_msg((IMS_CONTEXT_TYPE)rc,mi,recv);
				//m_sp_dbmgr->save_msg(mi);
				//header.status_code(200);
			}
			else
			{
				header.status_code(400);
			};
			*/

		};

	}
	else
	{
		LOG(error)<<"Queue_id = "<<request.m_nqueue<<" not exists!"<<ENDL;
		header.status_code(500);
	}

	response.encode(out);
	boost::shared_ptr<CSNetHandler> handler = boost::dynamic_pointer_cast<CSNetHandler>(spHandler);
	handler->send_packet(header,out);

}


void CSQueueMgr::processRequest(base::packet::Header& header,
		CSMsg_Request_Get_Msg& request,
		const boost::shared_ptr<base::NetHandlerBase>& spHandler)
{
	base::packet::OutPacket out;
	CSMsg_Response_Get_Msg response(request.m_nqueue,request.m_ncvsid);
	m_sp_dbmgr->get_msg(request.m_ncvsid,response.m_msg_list);
	response.encode(out);
	boost::shared_ptr<CSNetHandler> handler = boost::dynamic_pointer_cast<CSNetHandler>(spHandler);
	handler->send_packet(header,out);
}




}

