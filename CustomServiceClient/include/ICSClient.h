/*
 * ICSClient.h
 *
 *  Created on: 2010-4-19
 *      Author: root
 */

#ifndef ICSCLIENT_H_
#define ICSCLIENT_H_

#include <stddef.h>         // For size_t
#include <string>
#include <vector>
#include "type.h"
#include "CSMsg_Datatype.h"

#define CALLBACK

enum CS_CALLBACK_TYPE
{
	AGENT_PRESENCE_CALLBACK_TYPE,
	AGENT_ABSENCE_CALLBACK_TYPE,
	AGENT_REDIRECT_CALLBACK_TYPE,
	AGENT_GET_QUEUE_CALLBACK_TYPE,
	AGENT_GET_QUEUEAGENT_CALLBACK_TYPE,
	AGENT_ACCEPT_CALLBACK_TYPE,
	AGENT_REFUSE_CALLBACK_TYPE,
	AGENT_USERAMOUNT_CALLBACK_TYPE,
	AGENT_OFFLINE_CALLBACK_TYPE,	
	USER_JOIN_CALLBACK_TYPE,
	USER_DEPART_CALLBACK_TYPE,
	USER_SCORE_CALLBACK_TYPE,
	CONVERSATION_END_CALLBACK_TYPE,
	SEND_MSG_CALLBACK_TYPE,
	GET_MSG_CALLBACK_TYPE,
	
	CALLBACK_TYPE_COUNT
};


typedef void* CS_CONTEXT_TYPE;



/*
typedef void(CALLBACK* AGENT_PRESENCET_CALLBACK)(
	uint16 wReturnCode,		// 返回码
	CS_CONTEXT_TYPE dwContext,				// Context ID, if wReturnCode != 200, the followings are invalid	
	uint32 nQueue,
	uint32 nCount,
	USER_MULTPT_ID* ptr_Angent_info);
*/
typedef void(CALLBACK* AGENT_PRESENCE_CALLBACK)(
	uint16 wReturnCode,		// 返回码
	CS_CONTEXT_TYPE dwContext);				// Context ID, if wReturnCode != 200, the followings are invalid	

typedef void(CALLBACK* AGENT_ABSENCE_CALLBACK)(
	uint16 wReturnCode,		// 返回码
	CS_CONTEXT_TYPE dwContext);				// Context ID, if wReturnCode != 200, the followings are invalid	

typedef void(CALLBACK* AGENT_ACCEPT_CALLBACK)(
	uint16 wReturnCode,		// 返回码
	CS_CONTEXT_TYPE dwContext,				// Context ID, if wReturnCode != 200, the followings are invalid	
	const uint32& nQueueid,
	CONVERSATION_INFO& cvs_info);

typedef void(CALLBACK* AGENT_REFUSE_CALLBACK)(
	uint16 wReturnCode,		// 返回码
	CS_CONTEXT_TYPE dwContext,				// Context ID, if wReturnCode != 200, the followings are invalid	
	uint32 nQueueid);

typedef void(CALLBACK* AGENT_REDIRECT_CALLBACK)(
	uint16 wReturnCode,		// 返回码
	CS_CONTEXT_TYPE dwContext,				// Context ID, if wReturnCode != 200, the followings are invalid	
	uint32 nQueue);
/*
typedef void(CALLBACK* AGENT_REDIRECT_CALLBACK)(
	uint16 wReturnCode,		// 返回码
	CS_CONTEXT_TYPE dwContext,				// Context ID, if wReturnCode != 200, the followings are invalid	
	uint32 nQueue,
	uint32 nCount,
	QUEUE_INFO* ptr_Queue_info);
*/
typedef void(CALLBACK* AGENT_GET_QUEUE_CALLBACK)(
	uint16 wReturnCode,		// 返回码
	CS_CONTEXT_TYPE dwContext,				// Context ID, if wReturnCode != 200, the followings are invalid	
	const std::vector<QUEUE_INFO>& ptr_Queue_info);

typedef void(CALLBACK* AGENT_GET_QUEUEAGENT_CALLBACK)(
	uint16 wReturnCode,		// 返回码
	CS_CONTEXT_TYPE dwContext,				// Context ID, if wReturnCode != 200, the followings are invalid	
	const uint32& nQueue,
	const std::vector<AGENT_INFO>& ptr_Angent_info);

typedef void(CALLBACK* AGENT_OFFLINE_CALLBACK)(
	uint16 wReturnCode,		// 返回码
	CS_CONTEXT_TYPE dwContext);				// Context ID, if wReturnCode != 200, the followings are invalid	

typedef void(CALLBACK* AGENT_USERAMOUNT_CALLBACK)(
	uint16 wReturnCode,		// 返回码
	CS_CONTEXT_TYPE dwContext,				// Context ID, if wReturnCode != 200, the followings are invalid	
	uint32 nQueue,
	uint16 nNum);


typedef void (CALLBACK* USER_JOIN_MSGS_CALLBACK)(
	uint16 wReturnCode,		// 返回码
	CS_CONTEXT_TYPE dwContext,				// Context ID, if wReturnCode != 200, the followings are invalid
	uint32 nQueue,
	uint16 nPos);


typedef void(CALLBACK* USER_DEPART_MSGS_CALLBACK)(
	uint16 wReturnCode,		// 返回码
	CS_CONTEXT_TYPE dwContext				// Context ID, if wReturnCode != 200, the followings are invalid	
	);

typedef void (CALLBACK* USER_SCORE_MSGS_CALLBACK)(
	uint16 wReturnCode, 	// 返回码
	CS_CONTEXT_TYPE dwContext				// Context ID, if wReturnCode != 200, the followings are invalid	
	);


typedef void(CALLBACK* CONVERSATION_END_CALLBACK)(
	uint16 wReturnCode,		// 返回码
	CS_CONTEXT_TYPE dwContext);				// Context ID, if wReturnCode != 200, the followings are invalid	

typedef void(CALLBACK* SEND_MSG_CALLBACK)(
	uint16 wReturnCode,		// 返回码
	CS_CONTEXT_TYPE dwContext,				// Context ID, if wReturnCode != 200, the followings are invalid	
	const uint64& nReciverUid,
	const uint64& nReciverMultpt);

typedef void(CALLBACK* GET_MSG_CALLBACK)(
	uint16 wReturnCode,		// 返回码
	CS_CONTEXT_TYPE dwContext,
	const uint32& nQueue,
	const uint64& nCvsid,
	const std::vector<MSG_INFO>& msg_list
);				// Context ID, if wReturnCode != 200, the followings are invalid	




// OMS接口
class ICSClient
{
public:
	ICSClient() {}
	virtual ~ICSClient() {}

	virtual bool IsConnected() =0;
	virtual int Close() = 0;
	virtual int Release() = 0;

	virtual int Request_Agent_Presence(
		uint32 nQueueid,
		uint64 nAgentUid,
		uint64 nAgentMultpt,
		uint32 nCapacity,
		AGENT_PRESENCE_CALLBACK cb,	//�ص�����
		CS_CONTEXT_TYPE dwContext)=0;
	
	virtual int Request_Agent_Absence(
		uint32 nQueueid,
		uint64 nAgentUid,
		uint64 nAgentMultpt,
		AGENT_ABSENCE_CALLBACK cb,	//�ص�����
		CS_CONTEXT_TYPE dwContext)=0;

	virtual int Request_Agent_Accept(
		uint32 nQueueid,
		uint64 nAgentUid,
		uint64 nAgentMultpt,
		AGENT_ACCEPT_CALLBACK cb,	//�ص�����
		CS_CONTEXT_TYPE dwContext)=0;

	virtual int Request_Agent_Refuse(
		uint32 nQueueid,
		uint64 nAgentUid,
		uint64 nAgentMultpt,
		AGENT_REFUSE_CALLBACK cb,	//�ص�����
		CS_CONTEXT_TYPE dwContext)=0;

	virtual int Request_Agent_Redirect(
		uint32 nQueueid,
		uint32 nCvsid,
		uint64 nFromAgentUid,
		uint64 nFromAgentMultpt,
		uint64 nToAgentUid,
		uint64 nToAgentMultpt,
		AGENT_REDIRECT_CALLBACK cb,	//�ص�����
		CS_CONTEXT_TYPE dwContext)=0;

	virtual int Request_Agent_Get_Queue(
		uint64 nAgentUid,
		AGENT_GET_QUEUE_CALLBACK cb,	//�ص�����
		CS_CONTEXT_TYPE dwContext)=0;
	
	virtual int Request_Agent_Get_QueueAgent(
		uint32 nQueueid,
		AGENT_GET_QUEUEAGENT_CALLBACK cb,	//�ص�����
		CS_CONTEXT_TYPE dwContext)=0;

	virtual int Request_Agent_Offline(
		uint32 nQueueid,
		uint64 nUserUid,
		uint64 nUserMultpt,
		AGENT_OFFLINE_CALLBACK cb,	//�ص�����
		CS_CONTEXT_TYPE dwContext)=0;

	virtual int Request_Agent_UserNum(
		uint32 nQueueid,
		AGENT_USERAMOUNT_CALLBACK cb,	//�ص�����
		CS_CONTEXT_TYPE dwContext)=0;

	

	// �û�������ѯ
	virtual int Request_UserJoinMsg(
		uint32 nQueueid,
		uint64 nUserUid,
		uint64 nUserMultpt,
		uint32 nAddr,
		USER_JOIN_MSGS_CALLBACK cb,	//�ص�����
		CS_CONTEXT_TYPE dwContext)=0;

	// �û�������ѯ
	virtual int Request_UserDepartMsg(
		uint32 nQueueid,
		uint64 nUserUid,
		uint64 nUserMultpt,
		USER_DEPART_MSGS_CALLBACK cb,	//�ص�����
		CS_CONTEXT_TYPE dwContext)=0;

	//�û�����
	virtual int Request_UserScoreMsg(
		uint32 nQueueid,
		uint64 nCvsid,
		uint64 nUserUid,
		uint64 nUserMultpt,
		int8 nScore, //����
		USER_SCORE_MSGS_CALLBACK cb,
		CS_CONTEXT_TYPE dwContext)=0;

	virtual int Request_Conversation_End(
		uint32 nQueueid,
		uint64 nUserUid,
		uint64 nUserMultpt,
		uint64 nCvsid, //����
		CONVERSATION_END_CALLBACK cb,
		CS_CONTEXT_TYPE dwContext)=0;

	virtual int Request_Send_Msg(
		const uint32& nQueueid,
		const uint64& nCvsid,
		//const USER_MULTPT_ID& sender,
		const uint64& nSenderUid,
		const uint64& nSenderMultpt,
		const std::string& text,
		SEND_MSG_CALLBACK cb,
		CS_CONTEXT_TYPE dwContext)=0;

	virtual int Request_Get_Msg(
		const uint32& nQueueid,
		const uint64& nCvsid,
		GET_MSG_CALLBACK cb,
		CS_CONTEXT_TYPE dwContext)=0;





};

#endif /* ICSCLIENT_H_ */
