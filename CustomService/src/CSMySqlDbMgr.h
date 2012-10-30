#ifndef CUSTOM_SERVICE_CSMYSQLDBMGR_H
#define CUSTOM_SERVICE_CSMYSQLDBMGR_H
#include "CSMySqlConnMgr.h"
#include "ICSDbMgr.h"


namespace custom_service{

class CMySqlDbMgr : public IDbMgr , public CMySqlConnMgr
{
public:
	CMySqlDbMgr(){};
	virtual ~CMySqlDbMgr(){};

public:
	virtual bool init();

	virtual bool verify_queue_agent(const uint32& queue_id,AGENT& agent);
	virtual bool get_queue_info(const uint32& queue_id,std::string& name);
	virtual bool save_conversation_info(CONVERSATION* cvs);
	virtual bool save_conversation_score(const uint32& queue_id,const uint64& cvs_id,const int8& cvs_score);
	virtual bool get_queue_list(const uint64 agent_id,std::vector<QUEUE_INFO>& queue_list);
	virtual bool last_cvsid(uint64& last);
	virtual bool last_msgid(uint64& last);
	virtual bool save_msg(const MSG_INFO& msginfo);
	virtual bool get_msg(const uint64& cvs_id,std::vector<MSG_INFO>& msg_list) ;

};

}

#endif
