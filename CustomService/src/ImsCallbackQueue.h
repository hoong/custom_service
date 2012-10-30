/*
 * ImsCallbackQueue.h
 *
 *  Created on: 2010-5-18
 *      Author: root
 */

#ifndef IMS_CALLBACKQUEUE_H_
#define IMS_CALLBACKQUEUE_H_

#include <boost/thread/mutex.hpp>
#include <deque>
#include "UMID.h"
#include "Queue.h"
#include "ImsCallbackDef.h"

class CSImsCallbackInfo
{
public:
	CSImsCallbackInfo()
		: m_callback(NULL),
		m_index(0)
	{
	}

	CSImsCallbackInfo(IMS_CALLBACK_TYPE type, void* cb, IMS_CONTEXT_TYPE& context/*, const UMID& multptidreq*/)
		:	m_callback(cb),
		m_context(context),
		m_type(type),
		m_index(0)
		//m_multptidreq(multptidreq)
	{
		m_req_time = time(NULL);
	}

public:
	void* m_callback;
	IMS_CONTEXT_TYPE 		m_context;
	IMS_CALLBACK_TYPE 	m_type;
	uint32 	m_index;
	base::packet::Header m_header;

	//UMID 				m_multptidreq;
	time_t						 				m_req_time;
};


class CSImsCallbackQueue
{
public:
	CSImsCallbackQueue();
	virtual ~CSImsCallbackQueue();

public:
	// return the index pushed.
	uint32 AddCallback(IMS_CALLBACK_TYPE type, void* cb, IMS_CONTEXT_TYPE context/*, const UMID& multptidfrom*/);

	bool RemoveCallback(uint32 index);

	bool Deal(uint32 index, CSImsCallbackInfo& callback_info);

	void CheckQueue();
	void CancelQueue(uint16 statuscode);
	void Call(const CSImsCallbackInfo& callback_info, uint16 statuscode);

	void SetTimeout(uint32 milliseconds)
	{
		m_timeout_ms = milliseconds;
	}

	size_t GetQueueSize() { return m_queue.size(); }

protected:
	//ACE_Thread_Mutex m_mutex;
	boost::mutex m_mutex;
	typedef std::deque<CSImsCallbackInfo/*, netbaselib::ALLOCATOR<CSImsCallbackInfo>*/ > IMS_CALLBACK_QUEUE;
	IMS_CALLBACK_QUEUE m_queue;
	uint32 m_index;
	//ACE_Time_Value m_timeout_ms;
	time_t m_timeout_ms;
};

#endif /* IMS_CALLBACKQUEUE_H_ */
