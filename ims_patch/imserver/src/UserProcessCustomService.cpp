#include "UserProcess.h"
#include "UserManager.h"
#include "server/CSManager.h"
#include "CSMsg_Datatype.h"
#include "ICSClient.h"
#include "ims2s/IMS2SManager.h"

struct CS_SENDMSG_REQUEST_CONTEXT: public CS_REQUEST_CONTEXT
{
	CS_SENDMSG_REQUEST_CONTEXT():queue_id(0),cvs_id(0),text(""){};
	virtual ~CS_SENDMSG_REQUEST_CONTEXT(){};
	uint32 queue_id;
	uint64 cvs_id;
	std::string text;
};


//define callback

void cs_agent_presence_callback(
		uint16 wReturnCode,
		CS_CONTEXT_TYPE dwContext)
{
	ACE_Auto_Ptr<CS_REQUEST_CONTEXT> spcontext((CS_REQUEST_CONTEXT*)dwContext);
	if (!spcontext.get())
		return;

	USER_MULTPT_ID& multid = spcontext->request_multptid;
	uint32 seq = spcontext->request_seq;

	ND_LOG_TRACE("CUserProcess::cs_agent_presence_callback : return code=%u, uin=%Q \n", wReturnCode, multid.uin);

	USER_GUARD_EXIST(pUser, multid);
	if (NULL == pUser)
		return;

	netbaselib::CTcpS2COutPacket out;
	if (pUser->CreateS2CPacket(out, WY_CS_AGENT_PRESENCE, seq, wReturnCode))
	{
		out << (uint16)0; // 
		out.complete();

		pUser->SendPacketToClient(out);
	}
	return;

};

void cs_agent_absence_callback(
		uint16 wReturnCode,
		CS_CONTEXT_TYPE dwContext)
{
	ACE_Auto_Ptr<CS_REQUEST_CONTEXT> spcontext((CS_REQUEST_CONTEXT*)dwContext);
	if (!spcontext.get())
		return;

	USER_MULTPT_ID& multid = spcontext->request_multptid;
	uint32 seq = spcontext->request_seq;

	ND_LOG_TRACE("CUserProcess::cs_agent_absence_callback : return code=%u, uin=%Q \n", wReturnCode, multid.uin);

	USER_GUARD_EXIST(pUser, multid);
	if (NULL == pUser)
		return;

	netbaselib::CTcpS2COutPacket out;
	if (pUser->CreateS2CPacket(out, WY_CS_AGENT_ABSENCE, seq, wReturnCode))
	{
		out << (uint16)0; // 
		out.complete();

		pUser->SendPacketToClient(out);
	}
	return;
};

void cs_agent_accept_callback(
		uint16 wReturnCode,
		CS_CONTEXT_TYPE dwContext,
		const uint32& nQueueid,
		CONVERSATION_INFO& cvs_info)
{
	ACE_Auto_Ptr<CS_REQUEST_CONTEXT> spcontext((CS_REQUEST_CONTEXT*)dwContext);
	if (!spcontext.get())
		return;

	USER_MULTPT_ID& multid = spcontext->request_multptid;
	uint32 seq = spcontext->request_seq;
	uint16 imsid = cvs_info.user.multptinfo.imsid;

	USER_MULTPT_ID user;
	netbaselib::CTcpS2COutPacket send_content;

	ND_LOG_TRACE("CUserProcess::cs_agent_accept_callback : return code=%u, uin=%Q \n", wReturnCode, multid.uin);

	USER_GUARD_EXIST(pUser, multid);
	if (NULL == pUser)
	{
		wReturnCode = 400;
		goto RET;
	}

	//Relay Conversation Info to user
	user.Set(cvs_info.user.uin,cvs_info.user.multptvalue);
	send_content <<nQueueid <<cvs_info.conversation_id<<cvs_info.agent_name;
	send_content.complete();

	if (CONFIGURE_MANAGER::instance()->m_ims2sId == imsid)
	{

		USER_GUARD_EXIST(pUserDst, user);
		if (NULL == pUserDst)
		{
			wReturnCode = 400;
			goto RET;
		}
		if ( 0 != pUserDst->OnIMS2SFriendNotify(multid,
					WY_CS_CLIENT_NEWCVS,
					send_content.getData(),
					send_content.getDataLength()) 
				)
		{
			wReturnCode = 500;
			goto RET;
		}
	}
	else
	{

		IMS2S_GUARD_BY_ID(pIMS2SPeer, imsid);
		if (pIMS2SPeer)
		{
	
			if (!pIMS2SPeer->SendNotifyToFrds(multid, 
						WY_CS_CLIENT_NEWCVS,
					       	&user,
					       	1,
					       	send_content.getData(),
					       	send_content.getDataLength()))
			{
				wReturnCode = 500;
			}
		}
		else
		{
			wReturnCode = 500;
		};

	}


RET:
	netbaselib::CTcpS2COutPacket out;
	if (pUser->CreateS2CPacket(out, WY_CS_AGENT_ACCEPT, seq, wReturnCode))
	{
		if (wReturnCode != 200)
		{
			out << (uint16)0; // 
		}
		else
		{
			out <<nQueueid <<cvs_info.conversation_id<<cvs_info.user_addr;
		}
		out.complete();

		pUser->SendPacketToClient(out);
	}

	return;
};

void cs_agent_refuse_callback(
		uint16 wReturnCode,
		CS_CONTEXT_TYPE dwContext,
		uint32 nQueue)
{
	return;
};

void cs_agent_redirect_callback(
		uint16 wReturnCode,
		CS_CONTEXT_TYPE dwContext,
		uint32 nQueue)
{
	ACE_Auto_Ptr<CS_REQUEST_CONTEXT> spcontext((CS_REQUEST_CONTEXT*)dwContext);
	if (!spcontext.get())
		return;

	USER_MULTPT_ID& multid = spcontext->request_multptid;
	uint32 seq = spcontext->request_seq;

	ND_LOG_TRACE("CUserProcess::cs_agent_redirect_callback : return code=%u, uin=%Q \n", wReturnCode, multid.uin);

	USER_GUARD_EXIST(pUser, multid);
	if (NULL == pUser)
		return;

	netbaselib::CTcpS2COutPacket out;
	if (pUser->CreateS2CPacket(out, WY_CS_AGENT_REDIRECT, seq, wReturnCode))
	{
		if (wReturnCode != 200)
		{
			out << (uint16)0; // 
		}
		else
		{
			out <<nQueue ;
		}
		out.complete();

		pUser->SendPacketToClient(out);
	}
};

void cs_agent_get_queue_callback(
		uint16 wReturnCode,
		CS_CONTEXT_TYPE dwContext,
		const std::vector<QUEUE_INFO>& ptr_Queue_info)
{
	ACE_Auto_Ptr<CS_REQUEST_CONTEXT> spcontext((CS_REQUEST_CONTEXT*)dwContext);
	if (!spcontext.get())
		return;

	USER_MULTPT_ID& multid = spcontext->request_multptid;
	uint32 seq = spcontext->request_seq;

	ND_LOG_TRACE("CUserProcess::cs_agent_get_queue_callback : return code=%u, uin=%Q \n", wReturnCode, multid.uin);

	USER_GUARD_EXIST(pUser, multid);
	if (NULL == pUser)
		return;

	netbaselib::CTcpS2COutPacket out;
	if (pUser->CreateS2CPacket(out, WY_CS_AGENT_GET_QUEUE, seq, wReturnCode))
	{
		if (wReturnCode != 200)
		{
			out << (uint16)0; // 
		}
		else
		{
			uint16 cnt = ptr_Queue_info.size();
			out <<cnt;
			for (uint16 idx = 0;idx<cnt;idx++)
			{
				const QUEUE_INFO &qi = ptr_Queue_info[idx];
				out <<qi.wg_id <<qi.wg_name<<qi.queue_id<<qi.queue_name;
			}
		}
		out.complete();

		pUser->SendPacketToClient(out);
	}
};

void cs_agent_get_queueagent_callback(
		uint16 wReturnCode,
		CS_CONTEXT_TYPE dwContext,
		const uint32& nQueue,
		const std::vector<AGENT_INFO>& ptr_Agent_info)
{
	ACE_Auto_Ptr<CS_REQUEST_CONTEXT> spcontext((CS_REQUEST_CONTEXT*)dwContext);
	if (!spcontext.get())
		return;

	USER_MULTPT_ID& multid = spcontext->request_multptid;
	uint32 seq = spcontext->request_seq;

	ND_LOG_TRACE("CUserProcess::cs_agent_get_queueagent_callback : return code=%u, uin=%Q \n", wReturnCode, multid.uin);

	USER_GUARD_EXIST(pUser, multid);
	if (NULL == pUser)
		return;

	netbaselib::CTcpS2COutPacket out;
	if (pUser->CreateS2CPacket(out, WY_CS_AGENT_GET_QUEUEAGENT, seq, wReturnCode))
	{
		if (wReturnCode != 200)
		{
			out << (uint16)0; // 
		}
		else
		{
			uint16 cnt = ptr_Agent_info.size();
			out <<nQueue <<cnt;
			for (uint16 idx = 0;idx<cnt;idx++)
			{
				const AGENT_INFO &ai = ptr_Agent_info[idx];
				out << ai.mid.uin <<ai.mid.multptvalue <<ai.name;
			}
		}
		out.complete();

		pUser->SendPacketToClient(out);
	}

};

void cs_agent_offline_callback(
		uint16 wReturnCode,
		CS_CONTEXT_TYPE dwContext)
{
	return;
};

void cs_agent_useramount_callback(
		uint16 wReturnCode,
		CS_CONTEXT_TYPE dwContext,
		uint32 nQueue,
		uint16 nNum)
{
	ACE_Auto_Ptr<CS_REQUEST_CONTEXT> spcontext((CS_REQUEST_CONTEXT*)dwContext);
	if (!spcontext.get())
		return;

	USER_MULTPT_ID& multid = spcontext->request_multptid;
	uint32 seq = spcontext->request_seq;

	ND_LOG_TRACE("CUserProcess::cs_agent_useramount_callback : return code=%u, uin=%Q \n", wReturnCode, multid.uin);

	USER_GUARD_EXIST(pUser, multid);
	if (NULL == pUser)
		return;

	netbaselib::CTcpS2COutPacket out;
	if (pUser->CreateS2CPacket(out, WY_CS_AGENT_USERAMOUNT, seq, wReturnCode))
	{
		if (wReturnCode != 200)
		{
			out << (uint16)0; // 
		}
		else
		{
			out <<nQueue <<nNum ;
		}
		out.complete();

		pUser->SendPacketToClient(out);
	}
};

void cs_user_join_callback(
		uint16 wReturnCode,
		CS_CONTEXT_TYPE dwContext,
		uint32 nQueue,
		uint16 nPos)
{
	ACE_Auto_Ptr<CS_REQUEST_CONTEXT> spcontext((CS_REQUEST_CONTEXT*)dwContext);
	if (!spcontext.get())
		return;

	USER_MULTPT_ID& multid = spcontext->request_multptid;
	uint32 seq = spcontext->request_seq;

	ND_LOG_TRACE("CUserProcess::cs_user_join_callback : return code=%u, uin=%Q \n", wReturnCode, multid.uin);

	USER_GUARD_EXIST(pUser, multid);
	if (NULL == pUser)
		return;

	netbaselib::CTcpS2COutPacket out;
	if (pUser->CreateS2CPacket(out, WY_CS_USER_JOIN, seq, wReturnCode))
	{
		if (wReturnCode != 200)
		{
			out << (uint16)0; // 
		}
		else
		{
			out <<nQueue <<nPos;
		}
		out.complete();

		pUser->SendPacketToClient(out);
	}
	
};

void cs_user_depart_callback(
		uint16 wReturnCode,
		CS_CONTEXT_TYPE dwContext)
{
	ACE_Auto_Ptr<CS_REQUEST_CONTEXT> spcontext((CS_REQUEST_CONTEXT*)dwContext);
	if (!spcontext.get())
		return;

	USER_MULTPT_ID& multid = spcontext->request_multptid;
	uint32 seq = spcontext->request_seq;

	ND_LOG_TRACE("CUserProcess::cs_user_depart_callback : return code=%u, uin=%Q \n", wReturnCode, multid.uin);

	USER_GUARD_EXIST(pUser, multid);
	if (NULL == pUser)
		return;

	netbaselib::CTcpS2COutPacket out;
	if (pUser->CreateS2CPacket(out, WY_CS_USER_DEPART, seq, wReturnCode))
	{
		out << (uint16)0; // 
		out.complete();

		pUser->SendPacketToClient(out);
	}

};

void cs_user_score_callback(
		uint16 wReturnCode,
		CS_CONTEXT_TYPE dwContext)
{
	ACE_Auto_Ptr<CS_REQUEST_CONTEXT> spcontext((CS_REQUEST_CONTEXT*)dwContext);
	if (!spcontext.get())
		return;

	USER_MULTPT_ID& multid = spcontext->request_multptid;
	uint32 seq = spcontext->request_seq;

	ND_LOG_TRACE("CUserProcess::cs_user_score_callback : return code=%u, uin=%Q \n", wReturnCode, multid.uin);

	USER_GUARD_EXIST(pUser, multid);
	if (NULL == pUser)
		return;

	netbaselib::CTcpS2COutPacket out;
	if (pUser->CreateS2CPacket(out, WY_CS_USER_SCORE, seq, wReturnCode))
	{
		out << (uint16)0; // 
		out.complete();

		pUser->SendPacketToClient(out);
	}

};

void cs_conversation_end_callback(
		uint16 wReturnCode,
		CS_CONTEXT_TYPE dwContext)
{
	ACE_Auto_Ptr<CS_REQUEST_CONTEXT> spcontext((CS_REQUEST_CONTEXT*)dwContext);
	if (!spcontext.get())
		return;

	USER_MULTPT_ID& multid = spcontext->request_multptid;
	uint32 seq = spcontext->request_seq;

	ND_LOG_TRACE("CUserProcess::cs_conversation_end_callback : return code=%u, uin=%Q \n", wReturnCode, multid.uin);

	USER_GUARD_EXIST(pUser, multid);
	if (NULL == pUser)
		return;

	netbaselib::CTcpS2COutPacket out;
	if (pUser->CreateS2CPacket(out, WY_CS_CONVERSATION_END, seq, wReturnCode))
	{
		out << (uint16)0; // 
		out.complete();

		pUser->SendPacketToClient(out);
	}

};

void cs_send_msg_callback(
		uint16 wReturnCode,
		CS_CONTEXT_TYPE dwContext,
		const uint64& nReceiverUid,
		const uint64& nReceiverMultpt
		)
{
	ACE_Auto_Ptr<CS_REQUEST_CONTEXT> spcontext((CS_REQUEST_CONTEXT*)dwContext);
	if (!spcontext.get())
		return;

	CS_SENDMSG_REQUEST_CONTEXT* pContext = dynamic_cast<CS_SENDMSG_REQUEST_CONTEXT*>(spcontext.get());

	USER_MULTPT_ID& multid = pContext->request_multptid;
	uint32 seq = pContext->request_seq;

	USER_MULTPT_ID user;
	netbaselib::CTcpS2COutPacket send_content;
	uint16 imsid;

	ND_LOG_TRACE("CUserProcess::cs_send_msg_callback : return code=%u, uin=%Q \n", wReturnCode, multid.uin);

	USER_GUARD_EXIST(pUser, multid);
	if (NULL == pUser)
	{
		wReturnCode = 400;
		goto RET;
	}


	//Relay MSG to user
	user.Set(nReceiverUid,nReceiverMultpt);
	imsid = user.multptinfo.imsid;
	send_content <<pContext->queue_id<<pContext->cvs_id<<pContext->text;
	send_content.complete();

	if (CONFIGURE_MANAGER::instance()->m_ims2sId == imsid)
	{

		USER_GUARD_EXIST(pUserDst, user);
		if (NULL == pUserDst)
		{
			wReturnCode = 400;
			goto RET;
		}
		if ( 0 != pUserDst->OnIMS2SFriendNotify(multid,
					WY_CS_CLIENT_MSG,
					send_content.getData(),
					send_content.getDataLength()) 
				)
		{
			wReturnCode = 500;
			goto RET;
		}
	}
	else
	{

		IMS2S_GUARD_BY_ID(pIMS2SPeer, imsid);
		if (pIMS2SPeer)
		{
	
			if (!pIMS2SPeer->SendNotifyToFrds(multid, 
						WY_CS_CLIENT_MSG,
					       	&user,
					       	1,
					       	send_content.getData(),
					       	send_content.getDataLength()))
			{
				wReturnCode = 500;
			}
		}
		else
		{
			wReturnCode = 500;
		};

	}


RET:
	netbaselib::CTcpS2COutPacket out;
	if (pUser->CreateS2CPacket(out, WY_CS_SEND_MSG, seq, wReturnCode))
	{
		out << (uint16)0; // 
		out.complete();

		pUser->SendPacketToClient(out);
	}

};

void cs_get_msg_callback(
		uint16 wReturnCode,
		CS_CONTEXT_TYPE dwContext,
		const uint32& nQueue,
		const uint64& nCvsid,
		const std::vector<MSG_INFO>& msg_list)
{
	ACE_Auto_Ptr<CS_REQUEST_CONTEXT> spcontext((CS_REQUEST_CONTEXT*)dwContext);
	if (!spcontext.get())
		return;

	USER_MULTPT_ID& multid = spcontext->request_multptid;
	uint32 seq = spcontext->request_seq;

	ND_LOG_TRACE("CUserProcess::cs_get_msg_callback : return code=%u, uin=%Q \n", wReturnCode, multid.uin);

	USER_GUARD_EXIST(pUser, multid);
	if (NULL == pUser)
		return;

	netbaselib::CTcpS2COutPacket out;
	if (pUser->CreateS2CPacket(out, WY_CS_GET_MSG, seq, wReturnCode))
	{
		if (wReturnCode != 200)
		{
			out << (uint16)0; // 
		}
		else
		{
			uint16 cnt = msg_list.size();
			out <<nQueue <<nCvsid <<cnt;
			for (uint16 idx = 0;idx<cnt;idx++)
			{
				const MSG_INFO &mi = msg_list[idx];
				out << mi.msg_id <<mi.agent <<mi.user <<mi.send_flag <<mi.send_time <<mi.send_text;
			}
		}
		out.complete();

		pUser->SendPacketToClient(out);
	}

};


//CS Proc

int CUserProcess::OnCSAgentPresence(CAdaptedInPacket& in)
{
	ND_LOG_TRACE("CUserProcess::OnCSAgentPresence. uin=%Q.\n", GetUin());
	CS_GUARD(pCSClient);
	if (pCSClient)
	{
		uint32 queue_id,capacity;
		in >>queue_id>>capacity;

		ACE_Auto_Ptr<CS_REQUEST_CONTEXT> spcontext(new CS_REQUEST_CONTEXT);
		if (spcontext.get())
		{
			spcontext->request_multptid = GetMultPtID();
			spcontext->request_seq = in.GetSeq();

			int ret = pCSClient->Request_Agent_Presence(
					queue_id,
					GetMultPtID().uin,
					GetMultPtID().multptvalue,
					capacity,
					cs_agent_presence_callback,
					(CS_CONTEXT_TYPE)spcontext.get()
					);
	
			if (ret >=0 )
			{
				spcontext.release();
				return 0;
			}
			else 
			{
				ND_LOG_ERROR("CUserProcess::OnCSAgentPresence: pCSClient->Request_Agent_Presence return fail!\n");
			}
		}

	};

	// 失败
	netbaselib::CTcpS2COutPacket out;
	if (CreateS2CPacket(out,WY_CS_AGENT_PRESENCE , in.GetSeq(), 500))
	{
		out << (uint16)0;
		out.complete();

		SendPacketToClient(out);
	}
	return -1;

}


int CUserProcess::OnCSAgentAbsence(CAdaptedInPacket& in)
{
	ND_LOG_TRACE("CUserProcess::OnCSAgentAbsence. uin=%Q.\n", GetUin());
	CS_GUARD(pCSClient);
	if (pCSClient)
	{
		uint32 queue_id;
		in >>queue_id;

		ACE_Auto_Ptr<CS_REQUEST_CONTEXT> spcontext(new CS_REQUEST_CONTEXT);
		if (spcontext.get())
		{
			spcontext->request_multptid = GetMultPtID();
			spcontext->request_seq = in.GetSeq();

			int ret = pCSClient->Request_Agent_Absence(
					queue_id,
					GetMultPtID().uin,
					GetMultPtID().multptvalue,
					cs_agent_absence_callback,
					(CS_CONTEXT_TYPE)spcontext.get()
					);
	
			if (ret >=0 )
			{
				spcontext.release();
				return 0;
			}
			else 
			{
				ND_LOG_ERROR("CUserProcess::OnCSAgentAbsence: pCSClient->Request_Agent_Absence return fail!\n");
			}

		}
	};

	// 失败
	netbaselib::CTcpS2COutPacket out;
	if (CreateS2CPacket(out,WY_CS_AGENT_ABSENCE , in.GetSeq(), 500))
	{
		out << (uint16)0;
		out.complete();

		SendPacketToClient(out);
	}
	return -1;

}

int CUserProcess::OnCSAgentAccept(CAdaptedInPacket& in)
{
	ND_LOG_TRACE("CUserProcess::OnCSAgentAccept. uin=%Q.\n", GetUin());
	CS_GUARD(pCSClient);
	if (pCSClient)
	{
		uint32 queue_id;
		in >>queue_id;

		ACE_Auto_Ptr<CS_REQUEST_CONTEXT> spcontext(new CS_REQUEST_CONTEXT);
		if (spcontext.get())
		{
			spcontext->request_multptid = GetMultPtID();
			spcontext->request_seq = in.GetSeq();

			int ret = pCSClient->Request_Agent_Accept(
					queue_id,
					GetMultPtID().uin,
					GetMultPtID().multptvalue,
					cs_agent_accept_callback,
					(CS_CONTEXT_TYPE)spcontext.get()
					);
	
			if (ret >=0 )
			{
				spcontext.release();
				return 0;
			}
			else 
			{
				ND_LOG_ERROR("CUserProcess::OnCSAgentAccept: pCSClient->Request_Agent_Accept return fail!\n");
			}
		}

	};

	// 失败
	netbaselib::CTcpS2COutPacket out;
	if (CreateS2CPacket(out,WY_CS_AGENT_ACCEPT , in.GetSeq(), 500))
	{
		out << (uint16)0;
		out.complete();

		SendPacketToClient(out);
	}
	return -1;

}
int CUserProcess::OnCSAgentRefuse(CAdaptedInPacket& in)
{
	ND_LOG_TRACE("CUserProcess::OnCSAgentRefuse. uin=%Q.\n", GetUin());
	CS_GUARD(pCSClient);
	if (pCSClient)
	{
		uint32 queue_id;
		in >>queue_id;

		ACE_Auto_Ptr<CS_REQUEST_CONTEXT> spcontext(new CS_REQUEST_CONTEXT);
		if (spcontext.get())
		{
			spcontext->request_multptid = GetMultPtID();
			spcontext->request_seq = in.GetSeq();

			int ret = pCSClient->Request_Agent_Refuse(
					queue_id,
					GetMultPtID().uin,
					GetMultPtID().multptvalue,
					cs_agent_refuse_callback,
					(CS_CONTEXT_TYPE)spcontext.get()
					);
	
			if (ret >=0 )
			{
				spcontext.release();
				return 0;
			}
			else 
			{
				ND_LOG_ERROR("CUserProcess::OnCSAgentRefuse: pCSClient->Request_Agent_Refuse return fail!\n");
			}

		}
	};

	// 失败
	netbaselib::CTcpS2COutPacket out;
	if (CreateS2CPacket(out,WY_CS_AGENT_REFUSE, in.GetSeq(), 500))
	{
		out << (uint16)0;
		out.complete();

		SendPacketToClient(out);
	}
	return -1;

}

int CUserProcess::OnCSAgentRedirect(CAdaptedInPacket& in)
{
	ND_LOG_TRACE("CUserProcess::OnCSAgentRedirect. uin=%Q.\n", GetUin());

	CS_GUARD(pCSClient);
	if (pCSClient)
	{
		uint32 queue_id;
		uint64 cvs_id;
		USER_MULTPT_ID to;
		in >>queue_id >>cvs_id >>to.uin  >>to.multptvalue;

		ACE_Auto_Ptr<CS_REQUEST_CONTEXT> spcontext(new CS_REQUEST_CONTEXT);
		if (spcontext.get())
		{
			spcontext->request_multptid = GetMultPtID();
			spcontext->request_seq = in.GetSeq();

			int ret = pCSClient->Request_Agent_Redirect(
					queue_id,
					cvs_id,
					GetMultPtID().uin,
					GetMultPtID().multptvalue,
					to.uin,
					to.multptvalue,
					cs_agent_redirect_callback,
					(CS_CONTEXT_TYPE)spcontext.get()
					);
	
			if (ret >=0 )
			{
				spcontext.release();
				return 0;
			}
			else 
			{
				ND_LOG_ERROR("CUserProcess::OnCSAgentRedirect: pCSClient->Request_Agent_Redirectreturn fail!\n");
			}

		}
	};

	// 失败
	netbaselib::CTcpS2COutPacket out;
	if (CreateS2CPacket(out,WY_CS_AGENT_REDIRECT, in.GetSeq(), 500))
	{
		out << (uint16)0;
		out.complete();

		SendPacketToClient(out);
	}
	return -1;

}

int CUserProcess::OnCSAgentGetQueue(CAdaptedInPacket& in)
{
	ND_LOG_TRACE("CUserProcess::OnCSAgentRedirect. uin=%Q.\n", GetUin());

	CS_GUARD(pCSClient);
	if (pCSClient)
	{

		ACE_Auto_Ptr<CS_REQUEST_CONTEXT> spcontext(new CS_REQUEST_CONTEXT);
		if (spcontext.get())
		{
			spcontext->request_multptid = GetMultPtID();
			spcontext->request_seq = in.GetSeq();

			int ret = pCSClient->Request_Agent_Get_Queue(
					GetMultPtID().uin,
					cs_agent_get_queue_callback,
					(CS_CONTEXT_TYPE)spcontext.get()
					);
	
			if (ret >=0 )
			{
				spcontext.release();
				return 0;
			}
			else 
			{
				ND_LOG_ERROR("CUserProcess::OnCSAgentRedirect: pCSClient->Request_Agent_Redirectreturn fail!\n");
			}
		}

	};

	// 失败
	netbaselib::CTcpS2COutPacket out;
	if (CreateS2CPacket(out,WY_CS_AGENT_GET_QUEUE, in.GetSeq(), 500))
	{
		out << (uint16)0;
		out.complete();

		SendPacketToClient(out);
	}
	return -1;
}

int CUserProcess::OnCSAgentGetQueueagent(CAdaptedInPacket& in)
{
	ND_LOG_TRACE("CUserProcess::OnCSAgentGetQueueAgent. uin=%Q.\n", GetUin());

	CS_GUARD(pCSClient);
	if (pCSClient)
	{
		uint32 queue_id;
		in >>queue_id;

		ACE_Auto_Ptr<CS_REQUEST_CONTEXT> spcontext(new CS_REQUEST_CONTEXT);
		if (spcontext.get())
		{
			spcontext->request_multptid = GetMultPtID();
			spcontext->request_seq = in.GetSeq();

			int ret = pCSClient->Request_Agent_Get_QueueAgent(
					queue_id,
					cs_agent_get_queueagent_callback,
					(CS_CONTEXT_TYPE)spcontext.get()
					);
	
			if (ret >=0 )
			{
				spcontext.release();
				return 0;
			}
			else 
			{
				ND_LOG_ERROR("CUserProcess::OnCSAgentGetQueueAgent: pCSClient->Request_Agent_Get_QueueAgent return fail!\n");
			}

		}
	};

	// 失败
	netbaselib::CTcpS2COutPacket out;
	if (CreateS2CPacket(out,WY_CS_AGENT_GET_QUEUEAGENT, in.GetSeq(), 500))
	{
		out << (uint16)0;
		out.complete();

		SendPacketToClient(out);
	}
	return -1;

}

int CUserProcess::OnCSAgentUserAmount(CAdaptedInPacket& in)
{
	ND_LOG_TRACE("CUserProcess::OnCSAgentUserAmount. uin=%Q.\n", GetUin());

	CS_GUARD(pCSClient);
	if (pCSClient)
	{
		uint32 queue_id;
		in >>queue_id;

		ACE_Auto_Ptr<CS_REQUEST_CONTEXT> spcontext(new CS_REQUEST_CONTEXT);
		if (spcontext.get())
		{
			spcontext->request_multptid = GetMultPtID();
			spcontext->request_seq = in.GetSeq();

			int ret = pCSClient->Request_Agent_UserNum(
					queue_id,
					cs_agent_useramount_callback,
					(CS_CONTEXT_TYPE)spcontext.get()
					);
	
			if (ret >=0 )
			{
				spcontext.release();
				return 0;
			}
			else 
			{
				ND_LOG_ERROR("CUserProcess::OnCSAgentUserAmount: pCSClient->Request_Agent_UserNum return fail!\n"); 
			}

		}
	};

	// 失败
	netbaselib::CTcpS2COutPacket out;
	if (CreateS2CPacket(out,WY_CS_AGENT_USERAMOUNT, in.GetSeq(), 500))
	{
		out << (uint16)0;
		out.complete();

		SendPacketToClient(out);
	}
	return -1;

}

int CUserProcess::OnCSUserJoin(CAdaptedInPacket& in)
{
	ND_LOG_TRACE("CUserProcess::OnCSUserJoin. uin=%Q.\n", GetUin());

	CS_GUARD(pCSClient);
	if (pCSClient)
	{
		uint32 queue_id;
		in >>queue_id ;
		netbaselib::NET_ADDR addr;
		m_spnetHandle->GetAddr(addr);

		ACE_Auto_Ptr<CS_REQUEST_CONTEXT> spcontext(new CS_REQUEST_CONTEXT);
		if (spcontext.get())
		{
			spcontext->request_multptid = GetMultPtID();
			spcontext->request_seq = in.GetSeq();

			int ret = pCSClient->Request_UserJoinMsg(
					queue_id,
					GetMultPtID().uin,
					GetMultPtID().multptvalue,
					addr.ip,
					cs_user_join_callback,
					(CS_CONTEXT_TYPE)spcontext.get()
					);
	
			if (ret >=0 )
			{
				spcontext.release();
				return 0;
			}
			else 
			{
				ND_LOG_ERROR("CUserProcess::OnCSUserJoin: pCSClient->Request_UserJoinMsg return fail!\n"); 
			}

		}
	};

	// 失败
	netbaselib::CTcpS2COutPacket out;
	if (CreateS2CPacket(out,WY_CS_USER_JOIN, in.GetSeq(), 500))
	{
		out << (uint16)0;
		out.complete();

		SendPacketToClient(out);
	}
	return -1;

}

int CUserProcess::OnCSUserDepart(CAdaptedInPacket& in)
{
	ND_LOG_TRACE("CUserProcess::OnCSUserDepart. uin=%Q.\n", GetUin());

	CS_GUARD(pCSClient);
	if (pCSClient)
	{
		uint32 queue_id;
		in >>queue_id ;

		ACE_Auto_Ptr<CS_REQUEST_CONTEXT> spcontext(new CS_REQUEST_CONTEXT);
		if (spcontext.get())
		{
			spcontext->request_multptid = GetMultPtID();
			spcontext->request_seq = in.GetSeq();

			int ret = pCSClient->Request_UserDepartMsg(
					queue_id,
					GetMultPtID().uin,
					GetMultPtID().multptvalue,
					cs_user_depart_callback,
					(CS_CONTEXT_TYPE)spcontext.get()
					);
	
			if (ret >=0 )
			{
				spcontext.release();
				return 0;
			}
			else 
			{
				ND_LOG_ERROR("CUserProcess::OnCSUserDepart: pCSClient->Request_UserDepartMsg return fail!\n"); 
			}

		}
	};

	// 失败
	netbaselib::CTcpS2COutPacket out;
	if (CreateS2CPacket(out,WY_CS_USER_DEPART, in.GetSeq(), 500))
	{
		out << (uint16)0;
		out.complete();

		SendPacketToClient(out);
	}
	return -1;

}
int CUserProcess::OnCSUserScore(CAdaptedInPacket& in)
{
	ND_LOG_TRACE("CUserProcess::OnCSUserScore. uin=%Q.\n", GetUin());

	CS_GUARD(pCSClient);
	if (pCSClient)
	{
		uint32 queue_id;
		uint64 cvs_id;
		int8 score;
		in >>queue_id >>cvs_id>>score ;

		ACE_Auto_Ptr<CS_REQUEST_CONTEXT> spcontext(new CS_REQUEST_CONTEXT);
		if (spcontext.get())
		{
			spcontext->request_multptid = GetMultPtID();
			spcontext->request_seq = in.GetSeq();

			int ret = pCSClient->Request_UserScoreMsg(
					queue_id,
					cvs_id,
					GetMultPtID().uin,
					GetMultPtID().multptvalue,
					score,
					cs_user_score_callback,
					(CS_CONTEXT_TYPE)spcontext.get()
					);
	
			if (ret >=0 )
			{
				spcontext.release();
				return 0;
			}
			else 
			{
				ND_LOG_ERROR("CUserProcess::OnCSUserScore: pCSClient->Request_UserScoreMsg return fail!\n"); 
			}

		}
	};

	// 失败
	netbaselib::CTcpS2COutPacket out;
	if (CreateS2CPacket(out,WY_CS_USER_DEPART, in.GetSeq(), 500))
	{
		out << (uint16)0;
		out.complete();

		SendPacketToClient(out);
	}
	return -1;

}

int CUserProcess::OnCSConversationEnd(CAdaptedInPacket& in)
{
	ND_LOG_TRACE("CUserProcess::OnCSConversationEnd. uin=%Q.\n", GetUin());

	CS_GUARD(pCSClient);
	if (pCSClient)
	{
		uint32 queue_id;
		uint64 cvs_id;
		in >>queue_id >>cvs_id;

		ACE_Auto_Ptr<CS_REQUEST_CONTEXT> spcontext(new CS_REQUEST_CONTEXT);
		if (spcontext.get())
		{
			spcontext->request_multptid = GetMultPtID();
			spcontext->request_seq = in.GetSeq();

			int ret = pCSClient->Request_Conversation_End(
					queue_id,
					GetMultPtID().uin,
					GetMultPtID().multptvalue,
					cvs_id,
					cs_conversation_end_callback,
					(CS_CONTEXT_TYPE)spcontext.get()
					);
	
			if (ret >=0 )
			{
				spcontext.release();
				return 0;
			}
			else 
			{
				ND_LOG_ERROR("CUserProcess::OnCSConversationEnd: pCSClient->Request_Conversation_End return fail!\n"); 
			}

		}
	};

	// 失败
	netbaselib::CTcpS2COutPacket out;
	if (CreateS2CPacket(out,WY_CS_CONVERSATION_END, in.GetSeq(), 500))
	{
		out << (uint16)0;
		out.complete();

		SendPacketToClient(out);
	}
	return -1;

}

int CUserProcess::OnCSSendMsg(CAdaptedInPacket& in)
{
	ND_LOG_TRACE("CUserProcess::OnCSSendMsg. uin=%Q.\n", GetUin());

	CS_GUARD(pCSClient);
	if (pCSClient)
	{
		uint32 queue_id;
		uint64 cvs_id;
		std::string text;
		in >>queue_id >>cvs_id;

		ACE_Auto_Ptr<CS_REQUEST_CONTEXT> spcontext(new CS_SENDMSG_REQUEST_CONTEXT);
		if (spcontext.get())
		{
			CS_SENDMSG_REQUEST_CONTEXT* pContext = dynamic_cast<CS_SENDMSG_REQUEST_CONTEXT*>(spcontext.get());
			pContext->request_multptid = GetMultPtID();
			pContext->request_seq = in.GetSeq();
			pContext->queue_id=queue_id;
			pContext->cvs_id = cvs_id;
			pContext->text = text;

			int ret = pCSClient->Request_Send_Msg(
					queue_id,
					cvs_id,
					GetMultPtID().uin,
					GetMultPtID().multptvalue,
					text,
					cs_send_msg_callback,
					(CS_CONTEXT_TYPE)spcontext.get()
					);
	
			if (ret >=0 )
			{
				spcontext.release();
				return 0;
			}
			else 
			{
				ND_LOG_ERROR("CUserProcess::OnCSConversationEnd: pCSClient->Request_Conversation_End return fail!\n"); 
			}

		}
	};

	// 失败
	netbaselib::CTcpS2COutPacket out;
	if (CreateS2CPacket(out,WY_CS_CONVERSATION_END, in.GetSeq(), 500))
	{
		out << (uint16)0;
		out.complete();

		SendPacketToClient(out);
	}
	return -1;


}

int CUserProcess::OnCSGetMsg(CAdaptedInPacket& in)
{
	ND_LOG_TRACE("CUserProcess::OnCSGetMsg. uin=%Q.\n", GetUin());

	CS_GUARD(pCSClient);
	if (pCSClient)
	{
		uint32 queue_id;
		uint64 cvs_id;
		in >>queue_id >>cvs_id;

		ACE_Auto_Ptr<CS_REQUEST_CONTEXT> spcontext(new CS_REQUEST_CONTEXT);
		if (spcontext.get())
		{
			spcontext->request_multptid = GetMultPtID();
			spcontext->request_seq = in.GetSeq();

			int ret = pCSClient->Request_Get_Msg(
					queue_id,
					cvs_id,
					cs_get_msg_callback,
					(CS_CONTEXT_TYPE)spcontext.get()
					);
	
			if (ret >=0 )
			{
				spcontext.release();
				return 0;
			}
			else 
			{
				ND_LOG_ERROR("CUserProcess::OnCSGetMsg: pCSClient->Request_Get_Msg return fail!\n"); 
			}

		}
	};

	// 失败
	netbaselib::CTcpS2COutPacket out;
	if (CreateS2CPacket(out,WY_CS_GET_MSG, in.GetSeq(), 500))
	{
		out << (uint16)0;
		out.complete();

		SendPacketToClient(out);
	}
	return -1;

}


