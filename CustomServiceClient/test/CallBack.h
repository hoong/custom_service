#include "ICSClient.h"
#include "logger.h" 

struct sendmsg_context {
	uint32 nQueue;
	uint64 nCvsid;
	ICSClient* client;
};

struct accept_context {
	UMID agent;
	ICSClient* client;
};


void get_msg_callback(
	uint16 wReturnCode,
	CS_CONTEXT_TYPE dwContext,
	const uint32& nQueue,
	const uint64& nCvsid,
	const std::vector<MSG_INFO>& msg_list
)				// Context ID, if wReturnCode != 200, the followings are invalid	
{
	LOG(trace)<<"Get Msg:: queue="<<nQueue<<",cvsid="<<nCvsid<<ENDL;
	for (size_t i=0;i<msg_list.size();i++)
	{
		const MSG_INFO& mi = msg_list[i];
		LOG(trace)<<"msg_id="<<mi.msg_id<<",cvs_id="<<mi.cvs_id<<",agent="<<mi.agent<<",user="<<mi.user
					<<",send_flag="<<mi.send_flag<<",send_time="<<mi.send_time<<",send_text="<<mi.send_text<<ENDL;
	}

	LOG(trace)<<"Get Msg End"<<ENDL;

};



void send_msg_callback(
	uint16 wReturnCode,	
	CS_CONTEXT_TYPE dwContext,				// Context ID, if wReturnCode != 200, the followings are invalid	
	const uint64& uid,
	const uint64& multpt)
{
	LOG(fatal)<<"send_msg_callback:ret_code = "<<wReturnCode
	<<"receiver = {"<<uid <<","<<multpt<<"}"
	<<ENDL;
	
	sendmsg_context* sc_ptr =(sendmsg_context*)dwContext;

	int ret = sc_ptr->client->Request_Get_Msg(sc_ptr->nQueue,sc_ptr->nCvsid,get_msg_callback,NULL);
	if (ret ==0)
	{
		LOG(trace)<<"Send Successful"<<ENDL;
	};

	delete sc_ptr;




};



void agent_presence_callback(
	uint16 wReturnCode,		// 返回码
	CS_CONTEXT_TYPE dwContext)				// Context ID, if wReturnCode != 200, the followings are invalid	
{
	LOG(fatal)<< "agent_presence_callback:returncode="<<wReturnCode<<ENDL;

}

void agent_accept_callback(
	    uint16 wReturnCode,     
	    CS_CONTEXT_TYPE dwContext,             // Context ID, if wReturnCode != 200, the followings are invalid
	    const uint32& nQueueid,
	    CONVERSATION_INFO& cvs_info)
{
	LOG(fatal)<< "agent_accept_callback"<<",code="<<wReturnCode
		<<"Queue_id="<<nQueueid<<","
		<<"CVS_ID="<<cvs_info.conversation_id<<","
		<<"addr="<<cvs_info.user_addr<<","
		<<ENDL;

	const std::string text="Hello CustomService!";
	accept_context* p = (accept_context*) dwContext;

	sendmsg_context* sc1 = new sendmsg_context;
	sc1->client = p->client;
	sc1->nQueue = nQueueid;
	sc1->nCvsid = cvs_info.conversation_id;
	

	int ret = p->client->Request_Send_Msg(
		nQueueid,
		cvs_info.conversation_id,
		p->agent.uin,
		p->agent.multptvalue,
		text,
		send_msg_callback ,
		sc1);

	if (ret == 0)
	{
		LOG(fatal)<<"Send msg Successful"<<ENDL;
	};
	const std::string text1 = "Bye CustomService!";

	sendmsg_context* sc2 = new sendmsg_context;
	sc2->client = p->client;
	sc2->nQueue = nQueueid;
	sc2->nCvsid = cvs_info.conversation_id;
	
	ret = p->client->Request_Send_Msg(
		nQueueid,
		cvs_info.conversation_id,
		p->agent.uin,
		p->agent.multptvalue,
		text1,
		send_msg_callback ,
		sc2);

	if (ret == 0)
	{
		LOG(fatal)<<"Send msg Succesful"<<ENDL;
	};

	sleep(1);
}


void agent_absence_callback(
	uint16 wReturnCode,
	CS_CONTEXT_TYPE dwContext)				// Context ID, if wReturnCode != 200, the followings are invalid	
{

	LOG(fatal)<< "agent_absence_callback:returncode="<<wReturnCode<<ENDL;
}


void agent_refuse_callback(
	uint16 wReturnCode,		
	CS_CONTEXT_TYPE dwContext,				// Context ID, if wReturnCode != 200, the followings are invalid	
	uint32 nQueueid)
{

}

void agent_redirect_callback(
	uint16 wReturnCode,	
	CS_CONTEXT_TYPE dwContext,				// Context ID, if wReturnCode != 200, the followings are invalid	
	uint32 nQueue)
{

}

void agent_get_queue_callback(
	uint16 wReturnCode,	
	CS_CONTEXT_TYPE dwContext,				// Context ID, if wReturnCode != 200, the followings are invalid	
	const std::vector<QUEUE_DATA>& ptr_Queue_info)
{

}

void agent_get_queueagent_callback(
	uint16 wReturnCode,	
	CS_CONTEXT_TYPE dwContext,				// Context ID, if wReturnCode != 200, the followings are invalid	
	const uint32& nQueue,
	const std::vector<AGENT_INFO>& ptr_Angent_info)
{

}

void agent_offline_callback(
	uint16 wReturnCode,	
	CS_CONTEXT_TYPE dwContext)				// Context ID, if wReturnCode != 200, the followings are invalid	
{

}

void agent_useramount_callback(
	uint16 wReturnCode,		
	CS_CONTEXT_TYPE dwContext,				// Context ID, if wReturnCode != 200, the followings are invalid	
	uint32 nQueue,
	uint16 nNum)
{

}


void conversation_end_callback(
	uint16 wReturnCode,		
	CS_CONTEXT_TYPE dwContext)				// Context ID, if wReturnCode != 200, the followings are invalid	
{

}














static void UserJoinCallback(
	uint16 wReturnCode,		
	CS_CONTEXT_TYPE dwContext,				// Context ID, if wReturnCode != 200, the followings are invalid
	uint32 nQueue,
	uint16 nPos)
{
	LOG(fatal) << "UserJoinCallback!" << wReturnCode << ", nQueue:" << nQueue << ", nPos:" << nPos << ENDL;
}

static void UserDepartCallback(
	uint16	wReturnCode,			// 返回码
	CS_CONTEXT_TYPE dwContext)
{
	LOG(fatal) << "UserDepartCallback!" << wReturnCode << ENDL;
}

static void UserScoreCallback(
	uint16	wReturnCode,			// 返回码
	CS_CONTEXT_TYPE dwContext)
{
	LOG(fatal) << "UserScoreCallback!" << wReturnCode << ENDL;
}


