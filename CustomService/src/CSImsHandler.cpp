#include "CSImsHandler.h"
/*
#include "ims_msg/CSIMSMsg_Invite_List.h"
#include "ims_msg/CSIMSMsg_User_List.h"
#include "ims_msg/CSIMSMsg_User_Delcvs.h"
#include "ims_msg/CSIMSMsg_Agent_Delcvs.h"
#include "ims_msg/CSIMSMsg_User_Newcvs.h"
#include "ims_msg/CSIMSMsg_Relay_Msg.h"
*/
#include "CSThreadPool.h"
#include "logger.h"
#include "cmd.h"
#include <boost/foreach.hpp>




int CSImsHandler::handle_packet(base::packet::Header& header, base::packet::InPacket& in)
{
	boost::shared_ptr<base::packet::InPacket> in_ptr (new base::packet::InPacket());
	in_ptr->swap(in);
	custom_service::CS_IMS_THR_POOL::instance().schedule(&CSImsHandler::processResponse,this->shared_from_this(),header,in_ptr);

	return 1;

}


void CSImsHandler::processResponse(base::packet::Header& header, boost::shared_ptr<base::packet::InPacket>& in)
{
	CSImsCallbackInfo ci;
	m_callback_queue.Deal(header.seq(),ci);
	switch(ci.m_type)
	{
		case IMS_CALLBACKTYPE_RELAY_MSG:
			{
				IMS_CALLBACK_SEND_MSG cb = (IMS_CALLBACK_SEND_MSG)ci.m_callback;
				cb(ci.m_context,header.status_code());

			}
			break;

		default:
			LOG(error)<<"unknown callback type"<<ENDL;


	};
	

	//LOG(trace)<<"Receied Ret "<<header.cmd<<ENDL;


}

bool CSImsHandler::send_invite_list(const uint32& queue_id,const std::vector<UMID>& invite_list)
{
	//CSIMSMsg_Request_Invite_List request(queue_id);

	base::packet::Header header(WY_IMS2S_FRIEND_NOTIFY,0,SEQ_INDEX_NONE );
	UMID from;
	uint16 relay_cmd = WY_CS_CLIENT_INVITE;

	ICQ_DATA icq;
	base::packet::OutPacket out;

	icq <<queue_id;
	out << from << relay_cmd <<base::packet::as_container(invite_list) << icq;

	send_packet(header,out);

	/*
	request.m_invite_list.resize(invite_list.size());
	std::copy(invite_list.begin(),invite_list.end(),request.m_invite_list.begin());
	base::packet::OutPacket out;
	request.encode(out);
	send_packet(header,out);
	*/
	return true;
}

bool CSImsHandler::send_user_list(const uint32& queue_id,const std::vector<USER_INFO>& user_list)
{
	base::packet::Header header(WY_IMS2S_FRIEND_NOTIFY,0,SEQ_INDEX_NONE );
	UMID from;
	uint16 relay_cmd = WY_CS_CLIENT_USERINFO;

	BOOST_FOREACH(const USER_INFO& elm,user_list)
	{
		ICQ_DATA icq;
		base::packet::OutPacket out;

		std::vector<UMID> to;
		to.push_back(elm.mid);
	
		icq <<queue_id <<elm.position;
		out << from << relay_cmd <<base::packet::as_container(to) << icq;
	
		send_packet(header,out);
	};

	/*
	CSIMSMsg_Request_User_List request(queue_id);
	base::packet::Header header(WY_CS2IMS_USER_LIST);
	request.m_user_list.resize(user_list.size());
	std::copy(user_list.begin(),user_list.end(),request.m_user_list.begin());
	base::packet::OutPacket out;
	request.encode(out);
	send_packet(header,out);
	*/
	return true;
}

bool CSImsHandler::send_user_delcvs(const uint32& queue_id,const std::vector<USER_CONVERSATION>& usercvs_list)
{
	base::packet::Header header(WY_IMS2S_FRIEND_NOTIFY,0,SEQ_INDEX_NONE );
	UMID from ;
	uint16 relay_cmd = WY_CS_CLIENT_DELCVS;

	BOOST_FOREACH(const USER_CONVERSATION& elm,usercvs_list)
	{
		ICQ_DATA icq;
		base::packet::OutPacket out;

		std::vector<UMID> to;
		to.push_back(elm.mid);
	
		icq <<queue_id <<elm.conversation_id;
		out << from << relay_cmd <<base::packet::as_container(to) << icq;
	
		send_packet(header,out);
	};

	/*
	CSIMSMsg_Request_User_Delcvs request(queue_id);
	base::packet::Header header(WY_CS2IMS_USER_DELCVS);
	request.m_usercvs_list.resize(usercvs_list.size());
	std::copy(usercvs_list.begin(),usercvs_list.end(),request.m_usercvs_list.begin());
	base::packet::OutPacket out;
	request.encode(out);
	send_packet(header,out);
	*/
	return true;

}

bool CSImsHandler::send_agent_delcvs(const uint32& queue_id,const uint64& cvs_id,const UMID& agent)
{

	base::packet::Header header(WY_IMS2S_FRIEND_NOTIFY,0,SEQ_INDEX_NONE );
	UMID from;
	uint16 relay_cmd = WY_CS_CLIENT_DELCVS;

	ICQ_DATA icq;
	base::packet::OutPacket out;

	std::vector<UMID> to;
	to.push_back(agent);

	icq <<queue_id <<cvs_id;
	out << from << relay_cmd <<base::packet::as_container(to) << icq;

	send_packet(header,out);
	/*
	CSIMSMsg_Request_Agent_Delcvs request(queue_id,cvs_id);
	base::packet::Header header(WY_CS2IMS_AGENT_DELCVS);
	request.m_agent = agent;
	base::packet::OutPacket out;
	request.encode(out);
	send_packet(header,out);
	*/
	return true;
}


/*
//deprecated
bool CSImsHandler::send_user_newcvs(const uint32& queue_id,const uint64& cvs_id,const std::string &agent_name,const UMID& user)
{
	CSIMSMsg_Request_User_Newcvs request(queue_id,cvs_id,agent_name);
	base::packet::Header header(WY_CS2IMS_USER_NEWCVS);
	request.m_user= user;
	base::packet::OutPacket out;
	request.encode(out);
	send_packet(header,out);
	return true;
}


//deprecated
bool CSImsHandler::send_msg(IMS_CONTEXT_TYPE rc,const MSG_INFO& msg,const UMID& recv)
{
	CSIMSMsg_Request_Relay_Msg request;
	request.m_ncvsid= msg.cvs_id;
	request.m_recv=recv;
	request.m_text = msg.send_text;
	uint32 index = m_callback_queue.AddCallback(IMS_CALLBACKTYPE_RELAY_MSG,(void*)&relaymsg_callback,(IMS_CONTEXT_TYPE)rc);
	base::packet::Header header(WY_CS2IMS_MSG,0,index);
	base::packet::OutPacket out;
	request.encode(out);
	send_packet(header,out);
	return true;

}
*/


