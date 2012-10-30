#include "CSNetMgr.h"
#include "ConfigureMgr.h"
namespace custom_service
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

	if (-1 == this->reactor()->listen(CONFIGURE_MGR::instance().m_listen_port))
		return false;

	m_spthr.reset(new boost::thread(boost::bind(&CSNetMgr::loop, this)));

	return true;
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

