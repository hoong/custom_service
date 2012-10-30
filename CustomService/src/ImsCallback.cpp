#include "ImsCallback.h"
#include "CSMsg_Send_Msg.h"
#include "CSMySqlDbMgr.h"


void relaymsg_callback(const boost::shared_ptr<i_ims_context>& context,const uint16& status_code)
{

	boost::shared_ptr<relaymsg_context> r_context= boost::dynamic_pointer_cast<relaymsg_context>(context);
	CSMsg_Response_Send_Msg response;
	response.m_nqueue = r_context->queue_ptr->queue_id;
	base::packet::OutPacket out;

	if (status_code == 200)
	{
		if (r_context.get())
		{
			//boost::shared_ptr<custom_service::CMySqlDbMgr> dbmgr = boost::dynamic_pointer_cast<custom_service::CMySqlDbMgr>(r_context->dbmgr);
			r_context->dbmgr->save_msg(r_context->mi);
			r_context->header.status_code(200);
		}
		else
		{
			LOG(fatal)<<"Msg Relay Fail from "<<r_context->mi.send_text<<ENDL;

		}
	}
	else
	{
		LOG(error)<<"Msg Relay Fail from "<<r_context->mi.send_text<<ENDL;
		r_context->header.status_code(300);
	}

	response.encode(out);
	r_context->handler->send_packet(r_context->header,out);

	return;

}

