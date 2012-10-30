#include "CSNetHandler.h"
#include "logger.h"
//#include "command.h"
#include "cmd.h"
#include "CSThreadPool.h"
#include "CSQueueMgr.h"


namespace custom_service {

int CSNetHandler::open()
{
	LOG(trace)<<"OPEN: addr =" <<this->format_sockaddr()<<ENDL;
	return 0;
}

void CSNetHandler::handle_close()
{
	LOG(trace)<<"handle_close: addr="<<this->format_sockaddr()<<ENDL;
}

int CSNetHandler::handle_packet(base::packet::Header& header,base::packet::InPacket& in)
{
	boost::shared_ptr<base::packet::InPacket> in_ptr(new base::packet::InPacket());
	in_ptr->swap(in);
	CS_THR_POOL::instance().schedule(&CSNetHandler::processTask,this->shared_from_this(),header,in_ptr);
	//processTask(header,in);

	return 1;

}
	

int CSNetHandler::sendErrCode(uint16 cmd,uint32 seq,uint16 err)
{
	base::packet::Header header(cmd,err,seq);
	//base::packet::OutPacket out;
	return send_packet(header,err);
}

template <typename REQUEST,typename BSN_MGR>
void CSNetHandler::processBSNRequest(REQUEST& req,BSN_MGR& bsnmgr,base::packet::Header& header,base::packet::InPacket& in)
{
	try
	{
		req.decode(in);
		/*
		base::packet::OutPacket out;
		if (bsnmgr.processRequest(header,req,this->shared_from_this()))
		{
			send_packet(header,out);
		}
		*/
		bsnmgr.processRequest(header,req,this->shared_from_this());
	}
	catch(std::exception& e)
	{
		sendErrCode(header.cmd(),header.seq(),500);
		LOG(error)<<"processBSNRequest fail :cmd = "<<header.cmd()<<ENDL;
	}
}


void CSNetHandler::processTask(base::packet::Header& header,boost::shared_ptr<base::packet::InPacket>& in)
{

	switch(header.cmd())
	{
	case WY_CS_HEARTBEAT:
		{
			base::packet::OutPacket out;
			out<<*in;
			base::packet::Header header(WY_CS_HEARTBEAT,200,header.seq());
			send_packet(header,out);
		}
	break;

	case WY_CS_AGENT_PRESENCE:
		{
			CSMsg_Request_Agent_Presence request;
			processBSNRequest(request,CSMGR::instance(),header,*in);
		}
		break;
	case WY_CS_AGENT_ABSENCE:
		{
			CSMsg_Request_Agent_Absence request;
			processBSNRequest(request,CSMGR::instance(),header,*in);
		}
		break;
	case WY_CS_AGENT_REDIRECT:
		{
			CSMsg_Request_Agent_Redirect request;
			processBSNRequest(request,CSMGR::instance(),header,*in);
		}
		break;
	case WY_CS_AGENT_GET_QUEUE:
		{
			CSMsg_Request_Agent_Get_Queue request;
			processBSNRequest(request,CSMGR::instance(),header,*in);
		}
		break;
	case WY_CS_AGENT_GET_QUEUEAGENT:
		{
			CSMsg_Request_Agent_Get_QueueAgent request;
			processBSNRequest(request,CSMGR::instance(),header,*in);
		}
		break;
	case WY_CS_AGENT_ACCEPT:
		{
			CSMsg_Request_Agent_Accept request;
			processBSNRequest(request,CSMGR::instance(),header,*in);
		}
		break;
	case WY_CS_AGENT_REFUSE:
		{
			CSMsg_Request_Agent_Refuse request;
			processBSNRequest(request,CSMGR::instance(),header,*in);
		}
		break;
	case WY_CS_AGENT_USERAMOUNT:
		{
			CSMsg_Request_Agent_Useramount request;
			processBSNRequest(request,CSMGR::instance(),header,*in);
		}
		break;
	case WY_CS_AGENT_OFFLINE:
		{
			CSMsg_Request_Agent_Offline request;
			processBSNRequest(request,CSMGR::instance(),header,*in);
		}
		break;


	case WY_CS_USER_SCORE:
		{
			CSMsg_Request_User_Score request;
			processBSNRequest(request,CSMGR::instance(),header,*in);
		}
		break;
	case WY_CS_USER_JOIN:
		{
			CSMsg_Request_User_Join request;
			processBSNRequest(request,CSMGR::instance(),header,*in);
		}
		break;
	case WY_CS_USER_DEPART:
		{
			CSMsg_Request_User_Depart request;
			processBSNRequest(request,CSMGR::instance(),header,*in);
		}
		break;

	case WY_CS_CONVERSATION_END:
		{
			CSMsg_Request_Conversation_End request;
			processBSNRequest(request,CSMGR::instance(),header,*in);
		}
		break;

	case WY_CS_SEND_MSG:
		{
			CSMsg_Request_Send_Msg request;
			processBSNRequest(request,CSMGR::instance(),header,*in);
		}
		break;
	case WY_CS_GET_MSG:
		{
			CSMsg_Request_Get_Msg request;
			processBSNRequest(request,CSMGR::instance(),header,*in);
		}
		break;

	default:
		LOG(error)<<"unknown command!"<<ENDL;

	

	}

}




}

