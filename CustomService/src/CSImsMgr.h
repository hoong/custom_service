#ifndef CUSTOM_SERVICE_IMSMGR_H
#define CUSTOM_SERVICE_IMSMGR_H

#include "CSImsInfoMgr.h"
#include "CSImsHandler.h"
#include "net/reactor.h"
#include <boost/thread/detail/singleton.hpp>


typedef base::NetStrategyHandlerCreatorT<CSImsHandler> CSImsHandlerCreator;

class CSImsMgr
{
public:
	CSImsMgr(){};
	~CSImsMgr(){};

	bool init();
	bool start();
	void loop();
	bool get_handler(const uint16& imsid,boost::shared_ptr<CSImsHandler>& spHandler);

	base::NetReactor* reactor()
	{
		return m_spreactor.get();
	};



private:

	int connect(const uint8& imsid, boost::shared_ptr<CSImsHandler>& spHandler);

private:
	boost::scoped_ptr<base::NetReactor> m_spreactor;
	boost::scoped_ptr<boost::thread> m_spthr;
	boost::scoped_ptr<CSImsHandlerCreator> m_sp_handler_creator;
	CSImsInfoMgr m_imsinfo_mgr;
	CIMSMap m_imsmap;
	boost::mutex m_mutex;
};


typedef boost::detail::thread::singleton<CSImsMgr> CSIMSMGR;


#endif
