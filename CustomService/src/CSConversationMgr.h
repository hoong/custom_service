#ifndef CUSTOM_SERVICE_CSCONVERSATIONMGR_H_
#define CUSTOM_SERVICE_CSCONVERSATIONMGR_H_

#include "Conversation.h"
#include "ICSDbMgr.h"
namespace custom_service
{

class CSConversationMgr
{

public:
	CSConversationMgr() {};

	//uint64 insert(const uint32& queue,AGENT* agent,USER& user);
	CONVERSATION* insert(const CONVERSATION& cvs);
	bool erase(const uint64& cvs_id);
	bool erase(CONVERSATION* cvs_ptr);
	CONVERSATION* redirect(const uint64& cvs_id,AGENT* from,AGENT* to);
	bool score(const UMID& id,const uint64& cvs_id,const uint16& cvs_score);
	CONVERSATION* find(uint64 cvs_id);

private:
	std::map<uint64,CONVERSATION> m_conversation_pool;
	typedef std::pair<uint64,CONVERSATION> conversation_pool_value_type;
	typedef std::map<uint64,CONVERSATION>::iterator conversation_pool_value_ptr;
};


}

#endif
