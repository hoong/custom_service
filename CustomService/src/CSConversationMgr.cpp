#include "CSConversationMgr.h"

namespace custom_service
{

CONVERSATION* CSConversationMgr::find(uint64 cvs_id)
{
	conversation_pool_value_ptr it = m_conversation_pool.find(cvs_id);

	if (it == m_conversation_pool.end())
	{
		return NULL;
	}

	CONVERSATION* cvs_ptr = &(it->second);

	return cvs_ptr;

}

/*
uint64 CSConversationMgr::insert(const uint32& queue,AGENT* agent,USER& user)
{
	CONVERSATION cvs;
	cvs.cvs_id= CONVERSATION_SEQ.instance().next();
	cvs.queue_id=queue;
	cvs.agent = agent;
	cvs.user = user;

	std::pair<conversation_pool_value_ptr,bool> ret  = m_coversation_pool.insert(conversation_pool_value_type(cvs.cvs_id,cvs));

	if (ret.second == false)
	{
		return 0;
	};

	CONVERSATION* cvs_ptr = &(ret.first->second);


	return cvs_ptr->cvs_id;

}
*/

CONVERSATION* CSConversationMgr::insert(const CONVERSATION& cvs)
{
	std::pair<conversation_pool_value_ptr,bool> ret=m_conversation_pool.insert(conversation_pool_value_type(cvs.cvs_id,cvs));

	if (ret.second == false)
	{
		return NULL;
	};

	CONVERSATION* cvs_ptr = &(ret.first->second);
	cvs_ptr->_this = ret.first;


	return cvs_ptr;

}


bool CSConversationMgr::erase(const uint64& cvs_id)
{
	/*
	conversation_pool_value_ptr it = m_conversation_pool.find(cvs_id);

	if (it == m_covnersation_pool.end())
	{
		return false;
	}

	CONVERSATION* cvs_ptr = &(it->secont);
	*/

	CONVERSATION* cvs_ptr = find(cvs_id);
	if (cvs_ptr == NULL)
	{
		return false;
	}

	cvs_ptr->agent->remove_conversation(cvs_ptr->cvs_id);
	m_conversation_pool.erase(cvs_ptr->_this);

	return true;

}

bool CSConversationMgr::erase(CONVERSATION* cvs_ptr)
{
	if (cvs_ptr == NULL)
	{
		return false;
	};

	cvs_ptr->agent->remove_conversation(cvs_ptr->cvs_id);
	m_conversation_pool.erase(cvs_ptr->_this);
	return true;
}

bool CSConversationMgr::score(const UMID& id,const uint64& cvs_id,const uint16& cvs_score)
{
	CONVERSATION* cvs_ptr = find(cvs_id);

	if (cvs_ptr == NULL ||!(cvs_ptr->user.id.uin == id.uin))
	{
		return false;
	};

	cvs_ptr->score = cvs_score;
	return true;

}

CONVERSATION* CSConversationMgr::redirect(const uint64& cvs_id,AGENT* from,AGENT* to)
{

	if(from == NULL || to == NULL)
	{
		return NULL;
	}

	CONVERSATION* cvs_ptr = find(cvs_id);

	if(cvs_ptr == NULL && cvs_ptr->agent != from)
	{
		return NULL;
	}

	from->remove_conversation(cvs_id);
	cvs_ptr->agent = to;
	cvs_ptr->agent->add_conversation(cvs_id);

	return cvs_ptr; 
};

}

