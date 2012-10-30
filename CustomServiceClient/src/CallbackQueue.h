/*
 * CallbackQueue.h
 *
 *  Created on: 2010-4-20
 *      Author: root
 */

#ifndef CALLBACKQUEUE_H_
#define CALLBACKQUEUE_H_

#include "ICSClient.h"
#include <deque>
//#include "command.h"
#include "cmd.h"
#include <event2/util.h>
#include <sys/time.h>
//#include <time.h>
#include <boost/date_time.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/detail/singleton.hpp>
#include <evutil.h>
#include <event2/util.h>
#include	<vector>

namespace cs_client
{
const uint32 SEQ_INDEX_MASK = 0x7FFFFFFF;
const uint32 SEQ_INDEX_NONE = -1UL;


struct CALLBACK_INFO
{
	CALLBACK_INFO() :
		callback(NULL)
	{
	}

	CALLBACK_INFO(CS_CALLBACK_TYPE type_, void* cb_, CS_CONTEXT_TYPE context_, uint32 uid_=0, uint32 gid_=0, uint8 gtype_=0, uint32 other_=0, bool AgentState_ =1, uint16 Position_ = 0)
		:	callback(cb_),
		context(context_),
		type(type_),
		index(0),
		uid(uid_),
		gid(gid_),
		gtype(gtype_),
		other(0),
		AgentState(AgentState_),
		Position(Position_)
	{
		req_time = time(NULL);
	}

	void* callback;
	CS_CONTEXT_TYPE context;
	CS_CALLBACK_TYPE type;
	time_t req_time;
	uint32 index;

	uint32 uid;
	uint32 gid;
	uint8 gtype;
	uint32 other;

	bool AgentState;
	uint16 Position;
};

class CCallbackQueue
{
public:
		CCallbackQueue();
	virtual ~CCallbackQueue();

public:
	// return the index pushed.
	uint32 AddCallback(CS_CALLBACK_TYPE type, void* cb, CS_CONTEXT_TYPE context, uint32 uid=0, uint32 gid=0, uint8 gtype=0, uint32 other=0);

	bool RemoveCallback(uint32 index);

	bool Deal(uint32 index, CALLBACK_INFO& callback_info);

	void CheckQueue();
	void CancelQueue(uint16 statuscode);

	void Call(const CALLBACK_INFO& callback_info, uint16 statuscode);

	void SetTimeout(int time_out)
	{
		m_timeout_ms = time_out;

	}

	size_t GetQueueSize() { return m_queue.size(); }

protected:
	boost::mutex m_mutex; 
	typedef std::deque<CALLBACK_INFO, std::allocator<CALLBACK_INFO> > CALLBACK_QUEUE;
	CALLBACK_QUEUE m_queue;
	uint32 m_index;
	time_t  m_timeout_ms;
};

typedef boost::detail::thread::singleton<CCallbackQueue> CS_CB_QUEUE;

} /* end of namespace */


#endif /* CALLBACKQUEUE_H_ */
