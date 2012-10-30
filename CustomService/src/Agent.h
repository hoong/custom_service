#ifndef CUSTOM_SERVICE_AGENT_H_
#define CUSTOM_SERVICE_AGENT_H_
#include "CSMsg_Datatype.h"
#include <map>
#include <set>

namespace custom_service
{

enum AGENT_STATE
{
	AVAILABLE ,
	INVITING,
	REFUSE
};

class AGENT
{
public:
	UMID id;
	std::string name;

	AGENT_STATE state;

	uint16 capacity;
	uint16 load;

	AGENT* prev;
	AGENT* next;

	std::map<uint64,AGENT>::iterator _this;

	std::set<uint64> m_conversation_set;
	typedef std::set<uint64>::iterator CVS_PTR;


public:
	AGENT(): name(""),state(AVAILABLE),capacity(0),load(0),prev(NULL),next(NULL){};
	AGENT(UMID mid,uint16 agent_capacity): name(""),state(AVAILABLE),capacity(agent_capacity),load(0),prev(NULL),next(NULL)
	{
		id = mid;
	};
	~AGENT() {};



	inline void order_to_tail()
	{
		AGENT* agent_ptr ;
	
		while((agent_ptr = this->next)!= NULL )
		{
			if ( *agent_ptr>*this)
			{
				break;
			}
	
			agent_ptr->prev = this->prev;
			this->prev = agent_ptr;
			agent_ptr->prev->next = agent_ptr;
			
			this->next = agent_ptr->next;
			agent_ptr->next = this;
			if (this->next != NULL)
			{
				this->next->prev = this;
			}
			
		}
	
	};
	
	inline void order_to_head()
	{
	
		while(true)
		{
			AGENT* agent_ptr = this->prev;
			if (agent_ptr->prev == NULL || *this> *agent_ptr)
			{
				break;
			}
	
			this->prev = agent_ptr->prev;
			agent_ptr->prev = this;
			agent_ptr->prev->next = this;
	
			agent_ptr->next = this->next;
			this->next = agent_ptr;
			if (agent_ptr->next != NULL)
			{
				agent_ptr->next->prev = agent_ptr;
			}
	
		}
	
	};

	inline bool is_strong_aval()
	{
		return (capacity > load && state == AVAILABLE );
	};

	inline bool is_weak_aval()
	{
		return (capacity > load && state != INVITING);
	};

	inline bool operator>(const AGENT& rv)
	{
		return ( this->load >rv.load ||(this->load == rv.load && this->capacity <rv.capacity));
	};



	void operator>>(AGENT_INFO& ai);
	bool add_conversation(const uint64& cvs_id);
	bool remove_conversation(const uint64& cvs_id);
	bool erase();
	bool insert(AGENT* agent);

};



};

#endif
