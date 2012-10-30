#ifndef CSNETMGR_H_
#define CSNETMGR_H_

#include "CSNetHandler.h"
#include "net/reactor.h"
#include <boost/thread/detail/singleton.hpp>
#include <boost/thread.hpp>


namespace custom_service {

typedef base::NetStrategyHandlerCreatorT<CSNetHandler> CSNetHandlerCreator;

class CSNetMgr
{
public:
		CSNetMgr();
	virtual ~CSNetMgr();

	bool initialize();

	void join();

private:
	void loop();

	base::NetReactor* reactor()
	{
		return m_spreactor.get();
	};

private:
	boost::scoped_ptr<base::NetReactor> m_spreactor;
	boost::scoped_ptr<boost::thread> m_spthr;
	boost::scoped_ptr<CSNetHandlerCreator> m_sp_handler_creator;
};

typedef boost::detail::thread::singleton<CSNetMgr> CS_NET_MGR;

}

#endif /* CSNETMGR_H_ */
