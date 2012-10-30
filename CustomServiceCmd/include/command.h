#ifndef CUSTOM_SERVICE_COMMAND_H_
#define CUSTOM_SERVICE_COMMAND_H_

/*
1���ͷ�presence  CS_AGENT_PRESENCE  uid multpt queue_id   
2���ͷ�absence   CS_AGENT_ABSENCE   uid multpt queue_id
3���û�join	         CS_USER_JOIN    uid multpt add queue_id
4���û�depart      CS_USER_DEPART uid multpt queue_id
5���ͷ�redirect    CS_AGENT_REDIRECT uid1 multpt1 queue_id uid2 multpt2 session_id
6����ȡ�ͷ�����   CS_AGENT_GET_QUEUE  uid 
7���ͷ���ȡ���пͷ��б� CS_AGENT_QUEUE_AGENT queue_id
8���ͻ�����							CS_USER_SCORE uid,multpt,queue_id,session_id,score
9���Ự��ֹ							CS_SESSION_END uid,multpt,queue_id,session_id
*/


/*
 *
Agent
1���ͷ�presence  CS_AGENT_PRESENCE  uid multpt queue_id   
2���ͷ�absence   CS_AGENT_ABSENCE   uid multpt queue_id   
//3���ͷ�����Ự CS_AGENT_ASKFOR uid multpt queue_id  new 
4���ͷ����ܻỰ CS_AGENT_ACCEPT uid multpt queue_id   new
5���ͷ��ܾ��Ự CS_AGENT_REFUSE uid multpt queue_id   new
6���ͷ�redirect    CS_AGENT_REDIRECT uid1 multpt1 queue_id uid2 multpt2 session_id
7����ȡ�ͷ�����   CS_AGENT_GET_QUEUE  uid 
8���ͷ���ȡ���пͷ��б� CS_AGENT_QUEUE_AGENT queue_id
9���ͷ�����						CS_AGENT_OFFLINE uid queue_id  new
10���ͷ���ȡ�û����� CS_AGENT_GET_USERNUM  queue_id,size new 

User	
1���û�join	         CS_USER_JOIN    uid multpt add queue_id
2���û�depart      CS_USER_DEPART uid multpt queue_id
3���û�����							CS_USER_SCORE uid,multpt,queue_id,session_id,score

1���Ự��ֹ							CS_CONVERSATION_END uid,multpt,queue_id,session_id
*/

enum 
{
	CS_AGENT_PRESENCE = 0x0010,
	CS_AGENT_ABSENCE,
	CS_AGENT_ACCEPT,
	CS_AGENT_REFUSE,
	CS_AGENT_REDIRECT,
	CS_AGENT_GET_QUEUE,
	CS_AGENT_GET_QUEUEAGENT,
	CS_AGENT_USERAMOUNT,
	CS_AGENT_OFFLINE,

	CS_USER_SCORE = 0x0040,
	CS_USER_DEPART,
	CS_USER_JOIN,

	CS_CONVERSATION_END = 0x0070,

	CS_SEND_MSG,
	CS_GET_MSG,

	CS_HEARTBEAT = 0x0090,
	CS_HEARTBEAT_RET 
};


#endif
