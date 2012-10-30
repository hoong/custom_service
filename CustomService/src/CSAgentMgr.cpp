#include  "CSAgentMgr.h"
#include <iostream>
#include "logger.h"

namespace custom_service
{



AGENT* CSAgentMgr::insert(const AGENT& agent)
{
	std::pair<agent_pool_value_ptr,bool> ret = m_agent_pool.insert(agent_pool_value_type(agent.id.uin,agent));
	if (ret.second == false)
	{
		LOG(error)<<"agent:"<<agent.id.uin<<" exists"<<ENDL;
		return NULL;
	};

	AGENT* agent_ptr = &(ret.first->second);
	agent_ptr->_this = ret.first;

	head.insert(agent_ptr);

	return agent_ptr;

};


bool CSAgentMgr::erase(const UMID& id)
{

	agent_pool_value_ptr it= m_agent_pool.find(id.uin);
	AGENT*  agent = &(it->second);
	if (agent->id.multptvalue != id.multptvalue)
	{
		return false;
	}

	agent->erase();

	m_agent_pool.erase(it);

	return true;

};

bool CSAgentMgr::erase(AGENT*& agent)
{
	if (agent->load >0)
	{
		return false;
	};

	agent->erase();
	m_agent_pool.erase(agent->_this);
	
	return true;

};

AGENT* CSAgentMgr::accept(const UMID& id,bool passive)
{

	AGENT* agent = find(id);
	if (agent == NULL)
	{
		return NULL;
	}

	inviting_set_value_ptr it_set= m_inviting_set.find(id.uin);
	if (it_set == m_inviting_set.end() && passive)
	{
		return NULL;
	};

	m_inviting_set.erase(it_set);
	agent->state = AVAILABLE;
	return agent;

}


bool CSAgentMgr::refuse(const UMID& id)
{

	AGENT* agent = find(id);
	if (agent == NULL)
	{
		return false;
	}

	inviting_set_value_ptr it_set= m_inviting_set.find(id.uin);
	if (it_set == m_inviting_set.end())
	{
		return false;
	};

	m_inviting_set.erase(it_set);
	agent->state = REFUSE;
	return true;

}


uint16  CSAgentMgr::getlist(std::vector<AGENT_INFO>& ret_agent_list)
{
	AGENT_INFO ai;
	uint16 num = 0;
	AGENT* agent_ptr = head.next;
	while (agent_ptr != NULL)
	{
		num++;
		*agent_ptr >> ai;
		ret_agent_list.push_back(ai);
		agent_ptr = agent_ptr->next;
	}

	LOG(trace)<<"num="<<num<<endl;

	return num;
}; 


AGENT* CSAgentMgr::find(const UMID& id)
{
	agent_pool_value_ptr it= m_agent_pool.find(id.uin);
	if(it == m_agent_pool.end())
	{
		return NULL;
	};

	AGENT*  agent = &(it->second);
	if (agent->id.multptvalue != id.multptvalue)
	{
		return NULL;
	};

	return agent;

}

AGENT* CSAgentMgr::invite(AGENT* agent,bool force)
{
	AGENT* agent_ptr = (agent == NULL)?head.next:agent->next;
	while(agent_ptr != NULL)
	{
		if(agent_ptr->is_strong_aval() && m_inviting_set.insert(agent_ptr->id.uin).second)
		{
			agent_ptr->state = INVITING;
			break;
		}
		agent_ptr = agent_ptr->next;
	}

	return agent_ptr;
}

/*
bool CSAgentMgr::invite(AGENT* agent_ptr)
{
	if (agent_ptr->state == INVITING)
	{
		return false;
	}

	agent_ptr->state = INVITING;
	std::pair<inviting_set_value_ptr,bool> ret = m_inviting_set.insert(agent_ptr->id.uin);

	return ret.second;

}
*/




}
