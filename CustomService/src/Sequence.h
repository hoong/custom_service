#ifndef CUSTOM_SERVICE_SEQUENCE_H_
#define CUSTOM_SERVICE_SEQUENCE_H_

#include <boost/thread/detail/singleton.hpp>

namespace custom_service
{


class Sequence
{
public:

	Sequence()
	{
		//seq = 0;
		cvs_seq=0;
		msg_seq=0;
	};

	//temporarily invoked in CSQueueMgr::initialize
	inline void init_cvs(const uint64& last)
	{
		cvs_seq = last;
	};

	inline void init_msg(const uint64& last)
	{
		msg_seq = last;
	};


	inline uint64 next_cvs()
	{
		//...
		boost::mutex::scoped_lock lock(m_cvs_mutex);
		return ++cvs_seq;
	};

	inline uint64 next_msg()
	{
		//...
		boost::mutex::scoped_lock lock(m_msg_mutex);
		return ++msg_seq;
	};


private:
	uint64 cvs_seq;
	uint64 msg_seq;
	boost::mutex m_cvs_mutex;
	boost::mutex m_msg_mutex;

};

typedef boost::detail::thread::singleton<Sequence> SEQMGR;
//typedef boost::detail::thread::singleton<Sequence> CONVERSATION_SEQ;
//typedef boost::detail::thread::singleton<Sequence> MSG_SEQ;

}

#endif
