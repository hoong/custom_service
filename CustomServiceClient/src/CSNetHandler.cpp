#include "CSNetHandler.h"


namespace cs_client {

int CSNetHandler::open()
{
	LOG(trace)<<"OPEN: addr =" <<this->format_sockaddr()<<ENDL;
	return 0;
}

void CSNetHandler::handle_close()
{
	LOG(trace)<<"handle_close: addr="<<this->format_sockaddr()<<ENDL;
}

int CSNetHandler::handle_packet(base::packet::Header& header, base::packet::InPacket& in)
{
	
	boost::shared_ptr<base::packet::InPacket> in_ptr(new base::packet::InPacket());
	in_ptr->swap(in);
	CS_THR_POOL::instance().schedule(&CSNetHandler::processTask,this->shared_from_this(),header,in_ptr);

	return 1;

}
	

int CSNetHandler::sendErrCode(uint16 cmd,uint32 seq,uint16 err)
{
	base::packet::Header header(cmd,err,seq);
	base::packet::OutPacket out;
	return send_packet(header,out);
}

template <typename RESPONSE,typename BSN_MGR>
void CSNetHandler::processBSNResponse(RESPONSE& res,BSN_MGR& bsnmgr,base::packet::Header& header,base::packet::InPacket& in)
{
	try
	{
		res.decode(in);
		base::packet::OutPacket out;
		bsnmgr.processResponse(header,&out,res);
		//send_packet(header,out);
	}
	catch(std::exception& e)
	{
		sendErrCode(header.cmd(),header.seq(),500);
		LOG(error)<<"processBSNResponse fail :cmd = "<<header.cmd()<<ENDL;
	}
}


void CSNetHandler::processTask(base::packet::Header& header, boost::shared_ptr<base::packet::InPacket>& in)
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
			CSMsg_Response_Agent_Presence response;
			processBSNResponse(response,CS_PRO_TASK::instance(),header,*in);
		}
		break;
	case WY_CS_AGENT_ABSENCE:
		{
			CSMsg_Response_Agent_Absence response;
			processBSNResponse(response,CS_PRO_TASK::instance(),header,*in);
		}
		break;
	case WY_CS_AGENT_REDIRECT:
		{
			CSMsg_Response_Agent_Redirect response;
			processBSNResponse(response,CS_PRO_TASK::instance(),header,*in);
		}
		break;
	case WY_CS_AGENT_GET_QUEUE:
		{
			CSMsg_Response_Agent_Get_Queue response;
			processBSNResponse(response,CS_PRO_TASK::instance(),header,*in);
		}
		break;
	case WY_CS_AGENT_GET_QUEUEAGENT:
		{
			CSMsg_Response_Agent_Get_QueueAgent response;
			processBSNResponse(response,CS_PRO_TASK::instance(),header,*in);
		}
		break;
	case WY_CS_AGENT_ACCEPT:
		{
			CSMsg_Response_Agent_Accept response;
			processBSNResponse(response,CS_PRO_TASK::instance(),header,*in);
		}
		break;
	case WY_CS_AGENT_REFUSE:
		{
			CSMsg_Response_Agent_Refuse response;
			processBSNResponse(response,CS_PRO_TASK::instance(),header,*in);
		}
		break;
	case WY_CS_AGENT_USERAMOUNT:
		{
			CSMsg_Response_Agent_Useramount response;
			processBSNResponse(response,CS_PRO_TASK::instance(),header,*in);
		}
		break;
	case WY_CS_AGENT_OFFLINE:
		{
			CSMsg_Response_Agent_Offline response;
			processBSNResponse(response,CS_PRO_TASK::instance(),header,*in);
		}
		break;

	
	case WY_CS_USER_SCORE:
		{
			CSMsg_Response_User_Score response;
			processBSNResponse(response,CS_PRO_TASK::instance(),header,*in);
		}
		break;
	case WY_CS_USER_JOIN:
		{
			CSMsg_Response_User_Join response;
			processBSNResponse(response,CS_PRO_TASK::instance(),header,*in);
		}
		break;
	case WY_CS_USER_DEPART:
		{
			CSMsg_Response_User_Depart response;
			processBSNResponse(response,CS_PRO_TASK::instance(),header,*in);
		}
		break;

		
	case WY_CS_CONVERSATION_END:
		{
			CSMsg_Response_Conversation_End response;
			processBSNResponse(response,CS_PRO_TASK::instance(),header,*in);
		}
		break;
	case WY_CS_SEND_MSG:
		{
			CSMsg_Response_Send_Msg response;
			processBSNResponse(response,CS_PRO_TASK::instance(),header,*in);
		}
		break;
	case WY_CS_GET_MSG:
		{
			CSMsg_Response_Get_Msg response;
			processBSNResponse(response,CS_PRO_TASK::instance(),header,*in);
		}
		break;



	}

}

}

