#ifndef CUSTOM_SERVICE_CSAGENTMANAGER_H_
#define CUSTOM_SERVICE_CSAGENTMANAGER_H_

#include "CSMsg_Datatype.h"
#include "Agent.h"
#include <map>
#include <vector>

namespace custom_service
{

class CSAgentMgr
{

public:

	AGENT* insert(const AGENT& agent);
	bool erase(const UMID& id);
	bool erase(AGENT*& agent);
	AGENT* accept(const UMID& id,bool passive = true);
	bool refuse(const UMID& id);
	uint16 getlist(std::vector<AGENT_INFO>& ret_agent_list);
	AGENT* find(const UMID& id);
	//bool invite(AGENT* agent_ptr);
	AGENT* invite(AGENT* agent,bool force=false);
	

public:

	inline uint16 inviting_num()
	{
		return m_inviting_set.size();
	};

private:

	AGENT head;

	std::map<uint64,AGENT> m_agent_pool;
	std::set<uint64> m_inviting_set;

	typedef std::set<uint64>::iterator inviting_set_value_ptr;

	typedef std::pair<uint64,AGENT> agent_pool_value_type;
	typedef std::map<uint64,AGENT>::iterator agent_pool_value_ptr;
};

}


#endif
