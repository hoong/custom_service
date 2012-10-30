#include "CSProcessTask.h"


namespace cs_client
{
void CSProcessTask::processResponse(base::packet::Header& header,base::packet::OutPacket* out,CSMsg_Response_Agent_Presence& res)
{
	CALLBACK_INFO ci;
	uint32 seq = header.seq();

	if (CS_CB_QUEUE::instance().Deal(seq, ci))
	{
		if (ci.callback)
		{
			if(ci.type == AGENT_PRESENCE_CALLBACK_TYPE)
			{
				try
				{
					AGENT_PRESENCE_CALLBACK cb = (AGENT_PRESENCE_CALLBACK)ci.callback;
					cb(header.status_code(),
						ci.context);
				}
				catch (...)
				{
				}
			}
		}//end of if(ci.callback)
	}
}

void CSProcessTask::processResponse(base::packet::Header& header,base::packet::OutPacket* out,CSMsg_Response_Agent_Absence& res)
{
	CALLBACK_INFO ci;
	uint32 seq = header.seq();

	if (CS_CB_QUEUE::instance().Deal(seq, ci))
	{
		if (ci.callback)
		{
			if(ci.type == AGENT_ABSENCE_CALLBACK_TYPE)
			{
				try
				{
					AGENT_ABSENCE_CALLBACK cb = (AGENT_ABSENCE_CALLBACK)ci.callback;
					cb(header.status_code(),
						ci.context);
				}
				catch (...)
				{
				}
			}
		}//end of if(ci.callback)
	}
}

void CSProcessTask::processResponse(base::packet::Header& header,base::packet::OutPacket* out,CSMsg_Response_Agent_Redirect& res)
{
	CALLBACK_INFO ci;
	uint32 seq = header.seq();

	if (CS_CB_QUEUE::instance().Deal(seq, ci))
	{
		if (ci.callback)
		{
			if(ci.type == AGENT_REDIRECT_CALLBACK_TYPE)
			{
				try
				{
					AGENT_REDIRECT_CALLBACK cb = (AGENT_REDIRECT_CALLBACK)ci.callback;
					cb(header.status_code(),
						ci.context,res.m_nqueue);
				}
				catch (...)
				{
				}
			}
		}//end of if(ci.callback)
	}
}

void CSProcessTask::processResponse(base::packet::Header& header,base::packet::OutPacket* out,CSMsg_Response_Agent_Get_Queue& res)
{
	CALLBACK_INFO ci;
	uint32 seq = header.seq();

	if (CS_CB_QUEUE::instance().Deal(seq, ci))
	{
		if (ci.callback)
		{
			if(ci.type == AGENT_GET_QUEUE_CALLBACK_TYPE)
			{
				try
				{
					AGENT_GET_QUEUE_CALLBACK cb = (AGENT_GET_QUEUE_CALLBACK)ci.callback;
					cb(header.status_code(),
						ci.context,res.m_queue_list);
				}
				catch (...)
				{
				}
			}
		}//end of if(ci.callback)
	}
}

void CSProcessTask::processResponse(base::packet::Header& header,base::packet::OutPacket* out,CSMsg_Response_Agent_Get_QueueAgent& res)
{
	CALLBACK_INFO ci;
	uint32 seq = header.seq();

	if (CS_CB_QUEUE::instance().Deal(seq, ci))
	{
		if (ci.callback)
		{
			if(ci.type == AGENT_GET_QUEUEAGENT_CALLBACK_TYPE)
			{
				try
				{
					AGENT_GET_QUEUEAGENT_CALLBACK cb = (AGENT_GET_QUEUEAGENT_CALLBACK)ci.callback;
					cb(header.status_code(),
						ci.context,res.m_nqueue,res.m_agent_list);
				}
				catch (...)
				{
				}
			}
		}//end of if(ci.callback)
	}
}

void CSProcessTask::processResponse(base::packet::Header& header,base::packet::OutPacket* out,CSMsg_Response_Agent_Accept& res)
{
	CALLBACK_INFO ci;
	uint32 seq = header.seq();

	if (CS_CB_QUEUE::instance().Deal(seq, ci))
	{
		if (ci.callback)
		{
			if(ci.type == AGENT_ACCEPT_CALLBACK_TYPE)
			{
				try
				{
					AGENT_ACCEPT_CALLBACK cb = (AGENT_ACCEPT_CALLBACK)ci.callback;
					cb(header.status_code(),
						ci.context,
						res.m_nqueue,res.m_cvsinfo);
				}
				catch (...)
				{
				}
			}
		}//end of if(ci.callback)
	}
}

void CSProcessTask::processResponse(base::packet::Header& header,base::packet::OutPacket* out,CSMsg_Response_Agent_Refuse& res)
{
	CALLBACK_INFO ci;
	uint32 seq = header.seq();

	if (CS_CB_QUEUE::instance().Deal(seq, ci))
	{
		if (ci.callback)
		{
			if(ci.type == AGENT_REFUSE_CALLBACK_TYPE)
			{
				try
				{
					AGENT_REFUSE_CALLBACK cb = (AGENT_REFUSE_CALLBACK)ci.callback;
					cb(header.status_code(),
						ci.context,res.m_nqueue);
				}
				catch (...)
				{
				}
			}
		}//end of if(ci.callback)
	}
}

void CSProcessTask::processResponse(base::packet::Header& header,base::packet::OutPacket* out,CSMsg_Response_Agent_Useramount& res)
{
	CALLBACK_INFO ci;
	uint32 seq = header.seq();

	if (CS_CB_QUEUE::instance().Deal(seq, ci))
	{
		if (ci.callback)
		{
			if(ci.type == AGENT_USERAMOUNT_CALLBACK_TYPE)
			{
				try
				{
					AGENT_USERAMOUNT_CALLBACK cb = (AGENT_USERAMOUNT_CALLBACK)ci.callback;
					cb(header.status_code(),
						ci.context,res.m_nqueue,res.m_namount);
				}
				catch (...)
				{
				}
			}
		}//end of if(ci.callback)
	}
}

void CSProcessTask::processResponse(base::packet::Header& header,base::packet::OutPacket* out,CSMsg_Response_Agent_Offline& res)
{
	CALLBACK_INFO ci;
	uint32 seq = header.seq();

	if (CS_CB_QUEUE::instance().Deal(seq, ci))
	{
		if (ci.callback)
		{
			if(ci.type == AGENT_OFFLINE_CALLBACK_TYPE)
			{
				try
				{
					AGENT_OFFLINE_CALLBACK cb = (AGENT_OFFLINE_CALLBACK)ci.callback;
					cb(header.status_code(),
						ci.context);
				}
				catch (...)
				{
				}
			}
		}//end of if(ci.callback)
	}
}









void CSProcessTask::processResponse(base::packet::Header& header,base::packet::OutPacket* out,CSMsg_Response_User_Join& res)
{
	CALLBACK_INFO ci;
	
	uint32 seq = header.seq();

	printf("CS_USER_JOIN: recieve\n");
	if (CS_CB_QUEUE::instance().Deal(seq, ci))
	{
		if (ci.callback)
		{
			if(ci.type == USER_JOIN_CALLBACK_TYPE)
			{
				printf("res.m_nqueue = %d, res.pos = %d", res.m_nqueue, res.m_npos);
				try
				{
					USER_JOIN_MSGS_CALLBACK cb = (USER_JOIN_MSGS_CALLBACK)ci.callback;
					cb(header.status_code(),
						ci.context,
						res.m_nqueue,
						res.m_npos);
				}
				catch (...)
				{
				}
			}
		}//end of if(ci.callback)
	}


}

void CSProcessTask::processResponse(base::packet::Header& header,base::packet::OutPacket* out,CSMsg_Response_User_Depart& res)
{
	CALLBACK_INFO ci;
	uint32 seq = header.seq();

	if (CS_CB_QUEUE::instance().Deal(seq, ci))
	{
		if (ci.callback)
		{
			if(ci.type == USER_DEPART_CALLBACK_TYPE)
			{
				try
				{
					USER_DEPART_MSGS_CALLBACK cb = (USER_DEPART_MSGS_CALLBACK)ci.callback;
					cb(header.status_code(),
						ci.context);
				}
				catch (...)
				{
				}
			}
		}//end of if(ci.callback)
	}
}

void CSProcessTask::processResponse(base::packet::Header& header,base::packet::OutPacket* out,CSMsg_Response_User_Score& res)
{
	CALLBACK_INFO ci;
	uint32 seq = header.seq();

	if (CS_CB_QUEUE::instance().Deal(seq, ci))
	{
		if (ci.callback)
		{
			if(ci.type == USER_SCORE_CALLBACK_TYPE)
			{
				try
				{
					USER_SCORE_MSGS_CALLBACK cb = (USER_SCORE_MSGS_CALLBACK)ci.callback;
					cb(header.status_code(),
						ci.context);
				}
				catch (...)
				{
				}
			}
		}//end of if(ci.callback)
	}
}

void CSProcessTask::processResponse(base::packet::Header& header,base::packet::OutPacket* out,CSMsg_Response_Conversation_End& res)
{
	CALLBACK_INFO ci;
	uint32 seq = header.seq();

	if (CS_CB_QUEUE::instance().Deal(seq, ci))
	{
		if (ci.callback)
		{
			if(ci.type == CONVERSATION_END_CALLBACK_TYPE)
			{
				try
				{
					CONVERSATION_END_CALLBACK cb = (CONVERSATION_END_CALLBACK)ci.callback;
					cb(header.status_code(),
						ci.context);
				}
				catch (...)
				{
				}
			}
		}//end of if(ci.callback)
	}
}

void CSProcessTask::processResponse(base::packet::Header& header,base::packet::OutPacket* out,CSMsg_Response_Send_Msg& res)
{
	CALLBACK_INFO ci;
	uint32 seq = header.seq();

	if (CS_CB_QUEUE::instance().Deal(seq, ci))
	{
		if (ci.callback)
		{
			if(ci.type == SEND_MSG_CALLBACK_TYPE)
			{
				try
				{
					SEND_MSG_CALLBACK cb = (SEND_MSG_CALLBACK )ci.callback;
					cb(header.status_code(),
						ci.context,res.m_receiver.uin,res.m_receiver.multptvalue);
				}
				catch (...)
				{
				}
			}
		}//end of if(ci.callback)
	}
}

void CSProcessTask::processResponse(base::packet::Header& header,base::packet::OutPacket* out,CSMsg_Response_Get_Msg& res)
{
	CALLBACK_INFO ci;
	uint32 seq = header.seq();

	if (CS_CB_QUEUE::instance().Deal(seq, ci))
	{
		if (ci.callback)
		{
			if(ci.type == GET_MSG_CALLBACK_TYPE)
			{
				try
				{
					GET_MSG_CALLBACK cb = (GET_MSG_CALLBACK )ci.callback;
					cb(header.status_code(),
						ci.context,res.m_nqueue,res.m_ncvsid,res.m_msg_list);
				}
				catch (...)
				{
				}
			}
		}//end of if(ci.callback)
	}
}

/*
void CSProcessTask::processResponse(base::packet::Header& header,base::packet::InPacket& in)
{
	CALLBACK_INFO ci;
	
	uint16 cmd = header.cmd();
	uint32 seq = header.seq();

	switch(cmd)
	{
	case CS_USER_JOIN:
		{
			printf("CS_USER_JOIN: recieve\n");
			CSMsg_Response_User_Join res;
			if (CS_CB_QUEUE::instance().Deal(seq, ci))
			{
				if (ci.callback)
				{
						if(ci.type == USER_JOIN_CALLBACK_TYPE)
						{
							res.decode(in);
							printf("res.m_nqueue = %d, res.pos = %d", res.m_nqueue, res.m_npos));
							try
							{
								USER_JOIN_MSGS_CALLBACK cb = (USER_JOIN_MSGS_CALLBACK)ci.callback;
								cb(header.status_code(),
										ci.context,
										res.m_nqueue,
										res.m_npos);
							}
							catch (...)
							{
							}
							break;
						}
				}//end of if(ci.callback)
			}
		}
		break;
	case CS_USER_DEPART:
		{
			CSMsg_Response_User_Depart res;
			if (CS_CB_QUEUE::instance().Deal(seq, ci))
			{
				if (ci.callback)
				{
						if(ci.type == USER_DEPART_CALLBACK_TYPE)
						{
							res.decode(in);
							try
							{
								USER_DEPART_MSGS_CALLBACK cb = (USER_DEPART_MSGS_CALLBACK)ci.callback;
								cb(header.status_code(),
										ci.context);
							}
							catch (...)
							{
							}
							break;
						}
				}//end of if(ci.callback)
			}
		}
		break;
	case CS_USER_SCORE: 
		{
			CSMsg_Response_User_Score res;
			if (CS_CB_QUEUE::instance().Deal(seq, ci))
			{
				if (ci.callback)
				{
						if(ci.type == USER_SCORE_CALLBACK_TYPE)
						{
							res.decode(in);
							try
							{
								USER_SCORE_MSGS_CALLBACK cb = (USER_SCORE_MSGS_CALLBACK)ci.callback;
								cb(header.status_code(),
										ci.context);
							}
							catch (...)
							{
							}
							break;
						}
				}//end of if(ci.callback)
			}
		}
		break;
	
	case CS_HEARTBEAT:
		{
			//base::packet::OutPacket out;
			//out<<in;
			//base::packet::Header header(CS_HEARTBEAT_RET,200,header.seq());
			//send_packet(header,out);
		}
		break;
		
	}
}//end of processrequest
*/
}//end of namespace

