/*
 * CallbackQueue.cpp
 *
 *  Created on: 2010-4-20
 *      Author: root
 */

#include "CallbackQueue.h"
#include <vector>

namespace cs_client
{

CCallbackQueue::CCallbackQueue()
: m_index(0),m_timeout_ms(1000)
{
}

CCallbackQueue::~CCallbackQueue()
{
}

// return the index pushed. if return -1 fail!
uint32 CCallbackQueue::AddCallback(CS_CALLBACK_TYPE type, void* cb, CS_CONTEXT_TYPE context, uint32 uid, uint32 gid, uint8 gtype, uint32 other)
{
	if (!cb)
		return SEQ_INDEX_NONE;

	CALLBACK_INFO ci(type, cb, context, uid, gid, gtype, other);

	boost::mutex::scoped_lock lock(m_mutex);
	m_index = (m_index+1) & SEQ_INDEX_MASK;
	ci.index = m_index;

	m_queue.push_back(ci);

	return m_index;
}

bool CCallbackQueue::RemoveCallback(uint32 index)
{
	if (index == SEQ_INDEX_NONE)
		return true;

	boost::mutex::scoped_lock lock(m_mutex);

	if (m_queue.empty())
		return false;

	const CALLBACK_INFO& cifirst = m_queue.front();

	if (cifirst.index == index)
	{
		m_queue.pop_front();
	}
	else
	{
		uint32 d = (index - cifirst.index) & SEQ_INDEX_MASK;
		try
		{
			CALLBACK_INFO& ci = m_queue.at(d);
			ci.callback = NULL;  // set to invalid
		}
		catch (...)
		{
			return false;
		}
	}
	return true;
}

// 处理
bool CCallbackQueue::Deal(uint32 index, CALLBACK_INFO& callback_info)
{
	if (index == SEQ_INDEX_NONE)
		return true;

	//ND_LOG_TRACE("[OMS] Deal index=%u.", index);

	boost::mutex::scoped_lock lock(m_mutex);

	if (m_queue.empty())
		return false;

	const CALLBACK_INFO& cifirst = m_queue.front();
	if (cifirst.index == index)
	{
		callback_info = cifirst;
		m_queue.pop_front();

		while (!m_queue.empty())
		{
			if (m_queue.front().callback == NULL)
			{
				m_queue.pop_front();
			}
			else
				break;
		}
		return true;
	}
	else
	{
		uint32 d = (index - cifirst.index) & SEQ_INDEX_MASK;
		try
		{
			CALLBACK_INFO& ci = m_queue.at(d);
			if (ci.index == index)
			{
				callback_info = ci;
				ci.callback = NULL;  // to remove from queue later
				return true;
			}
		}
		catch (...)
		{
			return false;
		}
	}
	return false;
}

// 检查队列，处理超时的请求
void CCallbackQueue::CheckQueue()
{
	time_t now = time(NULL);

	std::vector<CALLBACK_INFO, std::allocator<CALLBACK_INFO> > vec;
	{
		boost::mutex::scoped_lock lock(m_mutex);

		// 检查超时的请求
		while (!m_queue.empty())
		{
			const CALLBACK_INFO& cifirst = m_queue.front();
			if (!cifirst.callback)
			{
				m_queue.pop_front();
				continue;
			}

			if(now -cifirst.req_time > m_timeout_ms)
			{
				vec.push_back(cifirst);
				m_queue.pop_front();
				continue;
			}
			else
				break;
		}
	}
	// 回调超时
	for (size_t i=0; i<vec.size(); ++i)
	{
		const CALLBACK_INFO& ci = vec[i];

		Call(ci, 504);
	}
}

void CCallbackQueue::CancelQueue(uint16 statuscode)
{
	boost::mutex::scoped_lock lock(m_mutex);

	CALLBACK_QUEUE::const_iterator cit;

	for (cit = m_queue.begin(); cit != m_queue.end(); ++cit)
	{
		const CALLBACK_INFO& ci = *cit;
		Call(ci, statuscode);
	}
}

void CCallbackQueue::Call(const CALLBACK_INFO& callback_info, uint16 statuscode)
{
	if (callback_info.callback)
	{
		switch (callback_info.type)
		{
		case USER_JOIN_CALLBACK_TYPE:
			{
				USER_JOIN_MSGS_CALLBACK cb = (USER_JOIN_MSGS_CALLBACK)callback_info.callback;
				cb(statuscode, callback_info.context,0,0);
			}
			break;
		case USER_DEPART_CALLBACK_TYPE:
			{
				USER_DEPART_MSGS_CALLBACK cb = (USER_DEPART_MSGS_CALLBACK)callback_info.callback;
				cb(statuscode, callback_info.context);
			}
			break;
		case USER_SCORE_CALLBACK_TYPE:
			{
				USER_SCORE_MSGS_CALLBACK cb = (USER_SCORE_MSGS_CALLBACK)callback_info.callback;
				cb(statuscode, callback_info.context);
			}
			break;
		case AGENT_PRESENCE_CALLBACK_TYPE:
			{
				AGENT_PRESENCE_CALLBACK cb = (AGENT_PRESENCE_CALLBACK)callback_info.callback;
				cb(statuscode, callback_info.context);
			}
			break;
		case AGENT_ABSENCE_CALLBACK_TYPE:
			{
				AGENT_ABSENCE_CALLBACK cb = (AGENT_ABSENCE_CALLBACK)callback_info.callback;
				cb(statuscode, callback_info.context);
			}
			break;
		case AGENT_REDIRECT_CALLBACK_TYPE:
			{
				AGENT_REDIRECT_CALLBACK cb = (AGENT_REDIRECT_CALLBACK)callback_info.callback;
				cb(statuscode, callback_info.context,0);
			}
			break;
		case AGENT_GET_QUEUE_CALLBACK_TYPE:
			{
				std::vector<QUEUE_INFO> queue_list;
				AGENT_GET_QUEUE_CALLBACK cb = (AGENT_GET_QUEUE_CALLBACK)callback_info.callback;
				cb(statuscode, callback_info.context,queue_list);
			}
			break;
		case AGENT_GET_QUEUEAGENT_CALLBACK_TYPE:
			{
				std::vector<AGENT_INFO> agent_list;
				AGENT_GET_QUEUEAGENT_CALLBACK cb = (AGENT_GET_QUEUEAGENT_CALLBACK)callback_info.callback;
				cb(statuscode, callback_info.context,0,agent_list);
			}
			break;
		case AGENT_ACCEPT_CALLBACK_TYPE:
			{
				CONVERSATION_INFO cvs_info;
				AGENT_ACCEPT_CALLBACK cb = (AGENT_ACCEPT_CALLBACK)callback_info.callback;
				cb(statuscode, callback_info.context,0,cvs_info);
			}
			break;

		case AGENT_REFUSE_CALLBACK_TYPE:
			{
				AGENT_REFUSE_CALLBACK cb = (AGENT_REFUSE_CALLBACK)callback_info.callback;
				cb(statuscode, callback_info.context,0);
			}
			break;
		case AGENT_USERAMOUNT_CALLBACK_TYPE:
			{
				AGENT_USERAMOUNT_CALLBACK cb = (AGENT_USERAMOUNT_CALLBACK)callback_info.callback;
				cb(statuscode, callback_info.context,0,0);
			}
			break;
		case AGENT_OFFLINE_CALLBACK_TYPE:
			{
				AGENT_OFFLINE_CALLBACK cb = (AGENT_OFFLINE_CALLBACK)callback_info.callback;
				cb(statuscode, callback_info.context);
			}
			break;
		case CONVERSATION_END_CALLBACK_TYPE:
			{
				CONVERSATION_END_CALLBACK cb = (CONVERSATION_END_CALLBACK)callback_info.callback;
				cb(statuscode, callback_info.context);
			}
			break;

		case SEND_MSG_CALLBACK_TYPE:
			{
				SEND_MSG_CALLBACK cb = (SEND_MSG_CALLBACK)callback_info.callback;
				cb(statuscode, callback_info.context,0,0);
			}
			break;

		case GET_MSG_CALLBACK_TYPE:
			{
				std::vector<MSG_INFO> msg_list;
				GET_MSG_CALLBACK cb = (GET_MSG_CALLBACK)callback_info.callback;
				cb(statuscode, callback_info.context,0,0,msg_list);
			}
			break;

	
		default:
			{
				//ND_LOG_ERROR("[OMS] CCallbackQueue::Call. callbacktype %u not found.\n", callback_info.type);
			}
		}
	}
}

} /* end of namespace */
