#ifndef CUSTOM_SERVICE_CONVERSATION_H_
#define CUSTOM_SERVICE_CONVERSATION_H_


#include "type.h"
#include "Agent.h"

namespace custom_service
{

struct USER
{
	UMID id;
	uint32 address;
};

struct CONVERSATION
{
	uint64 cvs_id;
	uint32 queue_id;
	AGENT* agent;
	USER user;
	int8 score;
	std::map<uint64,CONVERSATION>::iterator _this;

	CONVERSATION():cvs_id(0),queue_id(0),agent(NULL),score(-1) {}
	CONVERSATION(const uint64& id,const uint32& cvs_queue,AGENT* cvs_agent,USER& cvs_user):
		cvs_id(id),queue_id(cvs_queue),agent(cvs_agent),user(cvs_user),score(-1)
	{}
};




}

#endif
