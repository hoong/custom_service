#include "Agent.h"


namespace custom_service
{


bool AGENT::add_conversation(const uint64& cvs_id)
{
	std::pair<CVS_PTR,bool> ret = m_conversation_set.insert(cvs_id);
	if (ret.second == false)
	{
		return false;
	}
	//state = AVAILABLE;
	load++;
	order_to_tail();
	return true;
};

bool AGENT::remove_conversation(const uint64& cvs_id)
{
	size_t n = m_conversation_set.erase(cvs_id);
	if (n ==0)
	{
		return false;
	}
	load--;
	order_to_head();
	return true;
};


bool AGENT::erase()
{
	if (this->prev == NULL)
	{
		return false;
	}

	this->prev->next = this->next;
	if (this->next != NULL)
	{
		this->next->prev = this->prev;
	}
	this->prev = NULL;
	this->next = NULL;

	return true;
}


bool AGENT::insert(AGENT* agent)
{
	if ((agent->next = this->next) != NULL)
	{
		agent->next->prev = agent;
	}

	this->next = agent;
	agent->prev = this;

	agent->order_to_tail();

	return true;

}


void AGENT::operator>>(AGENT_INFO& ai)
{
	ai.mid = id;
	ai.name = name;
}



};

