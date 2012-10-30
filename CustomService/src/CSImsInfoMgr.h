/*
 * CSImsInfoMgr.h
 *
 */

#ifndef CUSTOM_SERVICE_CSIMSINFOMGR_H
#define CUSTOM_SERVICE_CSIMSINFOMGR_H
#include "CSMySqlConnMgr.h"
#include "ImsMap.h"

class CSImsInfoMgr: public CMySqlConnMgr
{
public:
	CSImsInfoMgr(){};
	virtual ~CSImsInfoMgr(){};
	int init();

	int load_allims();
	int get_ims(const uint16& imsid,std::string& imsaddr);

private:
	int load_ims(const uint16& imsid,std::string& imsaddr);
	CIMSIdMap m_imsmap;

};






#endif
