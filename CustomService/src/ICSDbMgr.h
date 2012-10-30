#ifndef CUSTOM_SERVICE_IDBMGR_H
#define CUSTOM_SERVICE_IDBMGR_H

#include "type.h"
#include "Conversation.h"
#include <vector>
namespace custom_service
{

class IDbMgr
{
public:
	IDbMgr(){};
	virtual ~IDbMgr(){};

	virtual bool init() = 0;

	virtual bool verify_queue_agent(const uint32& queue_id,AGENT& agent) = 0;
	virtual bool get_queue_info(const uint32& queue_id,std::string& name) =0 ;
	virtual bool save_conversation_info(CONVERSATION* cvs) = 0;
	virtual bool save_conversation_score(const uint32& queue_id,const uint64& cvs_id,const int8& cvs_score) = 0;
	virtual bool get_queue_list(const uint64 agent_id,std::vector<QUEUE_INFO>& queue_list) = 0;
	virtual bool last_cvsid(uint64& last) = 0;
	virtual bool last_msgid(uint64& last) = 0;
	virtual bool save_msg(const MSG_INFO& msginfo) = 0;
	virtual bool get_msg(const uint64& cvs_id,std::vector<MSG_INFO>& msg_list) = 0;


};

}

#endif
