#include "CSImsMgr.h"
#include "logger.h"



bool CSImsMgr::init()
{
	m_sp_handler_creator.reset(new CSImsHandlerCreator());
	m_spreactor.reset(new base::NetReactor(m_sp_handler_creator.get()));

	if (-1 == this->reactor()->init())
		return false;

	m_spthr.reset(new boost::thread(boost::bind(&CSImsMgr::loop, this)));
	m_imsinfo_mgr.load();

	return true;

}

int CSImsMgr::connect(const uint8& imsid, boost::shared_ptr<CSImsHandler>& spHandler)
{
	std::string ims_addr;
	timeval tv = {3600,0};

	if ( m_imsinfo_mgr.get_ims(imsid,ims_addr) == -1)
	{
		return -1;
	}
	boost::shared_ptr<base::NetHandlerBase> spHandlerbase;

	if (this->reactor()->connect(ims_addr.c_str(),&tv,spHandlerbase) == 0 )
	{
		spHandler = boost::dynamic_pointer_cast<CSImsHandler>(spHandlerbase);
		return 0;
	};

	LOG(error)<<"Connect to ims:"<<imsid<<" addr: "<<ims_addr<<"   Error"<<ENDL;
	return -1;
}

void CSImsMgr::loop()
{
	this->reactor()->run_loop();
}

bool CSImsMgr::start()
{
	if (m_spthr.get())
	{
		m_spthr->detach();
		return true;
	}
	return false;
}

bool CSImsMgr::get_handler(const uint16& imsid,boost::shared_ptr<CSImsHandler>& spHandler)
{
	if(!m_imsmap.GetHandler(imsid,spHandler))
	{
		boost::mutex::scoped_lock(m_mutex);
		if (connect(imsid,spHandler) == 0)
		{
			m_imsmap.InsertHandler(imsid,spHandler);
			return true;
		}

		return false;
	}
	return true;

}

