#ifndef CUSTOM_SERVICE_QUEUE_H_
#define CUSTOM_SERVICE_QUEUE_H_

#include "logger.h"
#include "CSAgentMgr.h"
#include "CSConversationMgr.h"
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <deque>

namespace custom_service
{

class QUEUE
{
public:
	QUEUE(IDbMgr* db):m_sp_dbmgr(db)
	{
		m_queue_mutex_ptr.reset(new boost::mutex);
	};

	~QUEUE() 
	{
	};

	inline void print()
	{
		std::cout<<"Queue:"<<queue_id<<","<<queue_name<<endl;
	};

	bool load(const uint32& id);

	bool agent_presence(const UMID& id,const uint16& capacity,std::vector<INVITE_INFO>& ret_invite_list);
	bool agent_absence(const UMID& id);
	bool agent_offline(const UMID& id,std::vector<USER_CONVERSATION>& ret_usercvs_list,std::vector<INVITE_INFO>& ret_invite_list);
	bool agent_accept(const UMID& id,CONVERSATION_INFO& cvs_info,std::vector<INVITE_INFO>& ret_invite_list,std::vector<USER_INFO>& ret_user_list);
	//bool agent_accept(const UMID& id,AGENT_CONVERSATION& agent_cvs,USER_CONVERSATION& user_cvs,std::vector<INVITE_INFO>& ret_invite_list,std::vector<USER_INFO>& ret_user_list);
	bool agent_refuse(const UMID& id,std::vector<INVITE_INFO>& ret_invite_list);
	bool agent_redirect(const uint64&  cvs_id,const UMID& from,const UMID& to,std::vector<INVITE_INFO>& ret_invite_list);
	uint16 agent_list(std::vector<AGENT_INFO>& ret_agent_list);

	uint16 user_join(const UMID& id,const uint32& addr,std::vector<INVITE_INFO>& ret_invite_list);
	bool user_depart(const UMID& id,std::vector<USER_INFO>& ret_user_list);
	bool user_score(const UMID& id,const uint64& cvs_id,const int8& score);
	uint16 user_amount();

	//bool conversation_end(const uint64& cvs_id,std::vector<INVITE_INFO>& ret_invite_list);
	bool conversation_end(const uint64& cvs_id,UMID& agent_mid,USER_CONVERSATION& usercvs,std::vector<INVITE_INFO>& ret_invite_list);

	inline CONVERSATION* get_cvs(const uint64& cvs_id)
	{
		return m_cvsmgr.find(cvs_id);
	};



private:
	uint16 do_invite(std::vector<INVITE_INFO>& ret_invite_list);
	uint16 get_userinfo(std::vector<USER_INFO>& ret_user_list,const uint16& pos=0);

public:
	uint32 queue_id;
	std::string queue_name;

private:
	boost::shared_ptr<boost::mutex> m_queue_mutex_ptr;

	CSAgentMgr m_agentmgr;
	CSConversationMgr m_cvsmgr;
	std::deque<USER> m_user_que;

	IDbMgr* m_sp_dbmgr;
};


}


#endif
