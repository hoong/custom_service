/*
 * CallbackQueue.cpp
 *
 *  Created on: 2010-5-18
 *      Author: root
 */

#include "ImsCallbackQueue.h"
#include "logger.h"
#include <vector>


CSImsCallbackQueue::CSImsCallbackQueue()
: m_index(0),
m_timeout_ms(10)
{
}

CSImsCallbackQueue::~CSImsCallbackQueue()
{

}

// return the index pushed.
uint32 CSImsCallbackQueue::AddCallback(IMS_CALLBACK_TYPE type, void* cb, IMS_CONTEXT_TYPE context/*, const UMID& multptidfrom*/)
{
	if (!cb)
		return SEQ_INDEX_NONE;

	CSImsCallbackInfo ci(type, cb, context/*, multptidfrom*/);

	boost::mutex::scoped_lock lock(m_mutex);
	m_index = (m_index+1) & SEQ_INDEX_MASK;
	ci.m_index = m_index;

	m_queue.push_back(ci);

	return m_index;
}

bool CSImsCallbackQueue::RemoveCallback(uint32 index)
{
	if (index == SEQ_INDEX_NONE)
		return true;

	//ACE_GUARD_RETURN(ACE_Thread_Mutex, ace_mon, m_mutex, false);
	boost::mutex::scoped_lock lock(m_mutex);

	if (m_queue.empty())
		return false;

	const CSImsCallbackInfo& cifirst = m_queue.front();

	if (cifirst.m_index == index)
	{
		m_queue.pop_front();
	}
	else
	{
		uint32 d = (index - cifirst.m_index) & SEQ_INDEX_MASK;
		try
		{
			CSImsCallbackInfo& ci = m_queue.at(d);
			ci.m_callback = NULL;  // set to invalid
		}
		catch (...)
		{
			return false;
		}
	}
	return true;
}

// 处理
bool CSImsCallbackQueue::Deal(uint32 index, CSImsCallbackInfo& callback_info)
{
	if (index == SEQ_INDEX_NONE)
		return true;

	LOG(trace)<<"CSImsCallbackQueue: Deal index="<< index<<ENDL;

	boost::mutex::scoped_lock lock(m_mutex);
	//ACE_GUARD_RETURN(ACE_Thread_Mutex, ace_mon, m_mutex, false);

	if (m_queue.empty())
		return false;

	const CSImsCallbackInfo& cifirst = m_queue.front();
	if (cifirst.m_index == index)
	{
		callback_info = cifirst;
		m_queue.pop_front();

		while (!m_queue.empty())
		{
			if (m_queue.front().m_callback == NULL)
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
		uint32 d = (index - cifirst.m_index) & SEQ_INDEX_MASK;
		try
		{
			CSImsCallbackInfo& ci = m_queue.at(d);
			if (ci.m_index == index)
			{
				callback_info = ci;
				ci.m_callback = NULL;  // set to invalid
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
void CSImsCallbackQueue::CheckQueue()
{
	//ACE_Time_Value now = ACE_OS::gettimeofday();
	time_t now = time(NULL);

	std::vector<CSImsCallbackInfo/*, netbaselib::ALLOCATOR<CSImsCallbackInfo>*/ > vec;
	{
		//ACE_GUARD(ACE_Thread_Mutex, ace_mon, m_mutex);
		boost::mutex::scoped_lock lock(m_mutex);

		// 检查超时的请求
		while (!m_queue.empty())
		{
			const CSImsCallbackInfo& cifirst = m_queue.front();
			if (!cifirst.m_callback)
			{
				m_queue.pop_front();
				continue;
			}

			if (now - cifirst.m_req_time > m_timeout_ms)
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
		const CSImsCallbackInfo& ci = vec[i];

		Call(ci, 504);
	}
}


void CSImsCallbackQueue::CancelQueue(uint16 statuscode)
{
	//ACE_GUARD(ACE_Thread_Mutex, ace_mon, m_mutex);
	boost::mutex::scoped_lock lock(m_mutex);

	IMS_CALLBACK_QUEUE::const_iterator cit;

	for (cit = m_queue.begin(); cit != m_queue.end(); ++cit)
	{
		const CSImsCallbackInfo& ci = *cit;
		Call(ci, statuscode);
	}
}

void CSImsCallbackQueue::Call(const CSImsCallbackInfo& callback_info, uint16 statuscode)
{
	if (callback_info.m_callback)
	{
		switch (callback_info.m_type)
		{
		case IMS_CALLBACKTYPE_RELAY_MSG:
			{
				IMS_CALLBACK_SEND_MSG cb = (IMS_CALLBACK_SEND_MSG)callback_info.m_callback;
				cb(callback_info.m_context,statuscode);
			}
			break;

			/*
		case IMS_MSG_RELAY_CALLBACK_TYPE:
			{
				IMS_MSG_RELAY_CALLBACK cb = (IMS_MSG_RELAY_CALLBACK)callback_info.m_callback;
				cb(callback_info.m_multptidreq,
						callback_info.m_context,
						statuscode,
						NULL,
						0);
			}
			break;

		case IMS_MSG_NOTIFY_CALLBACK_TYPE:
			{
				IMS_MSG_NOTIFY_CALLBACK cb = (IMS_MSG_NOTIFY_CALLBACK)callback_info.m_callback;
				cb(callback_info.m_multptidreq,
						callback_info.m_context,
						statuscode,
						NULL,
						0);
			}
			break;
			*/

		default:
			{
				LOG(error)<<"CSImsCallbackQueue::Call. callbacktype "<< callback_info.m_type
					<<"not found"<<ENDL;
			}
		}
	}
}
