#include "CSNetMgr.h"

namespace cs_client
{

CSNetMgr::CSNetMgr()
{
}

CSNetMgr::~CSNetMgr()
{
}

bool CSNetMgr::initialize()
{								
	m_sp_handler_creator.reset(new CSNetHandlerCreator());
	m_spreactor.reset(new base::NetReactor(m_sp_handler_creator.get()));

	if (-1 == this->reactor()->init())
		return false;
	
	
	m_spthr.reset(new boost::thread(boost::bind(&CSNetMgr::loop, this)));

	return true;
}

bool CSNetMgr::connect(const char* address, const timeval* tv, boost::shared_ptr<base::NetHandlerBase>& spNetHandlerBase)
{
	if (!m_spreactor->connect(address, tv, spNetHandlerBase))
	{
		return false;
	}
	else
	{
		return true;
	}

}
void CSNetMgr::join()
{
	if (m_spthr.get())
	{
		m_spthr->join();
		m_spthr.reset(NULL);
	}
}

void CSNetMgr::loop()
{
	this->reactor()->run_loop();
}

}

