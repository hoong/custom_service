/*
 * ImsMap.h
 *
 *  Created on: 2009-11-4
 *      Author: root
 */

#ifndef CUSTOM_SERVICE_IMSMAP_H_
#define CUSTOM_SERVICE_IMSMAP_H_

//#include "netbaselib/include/NetAddr.h"
//#include "../RefMap.h"
//#include "IMS2SSvcHandler.h"
#include "CSImsHandler.h"
#include "tbb/concurrent_hash_map.h"
#include <boost/shared_ptr.hpp>


// CIMS2SMap class

/*
class CIMS2SMap : public CRefConcurrentHashMap<netbaselib::NET_ADDR, CIMS2SSvcHandler*, TBB_HASH_COMPARATOR<netbaselib::NET_ADDR> >
{
public:
	CIMS2SMap() {}
	virtual ~CIMS2SMap() {}


};
*/


// CIMSIdMap class

class CIMSIdMap : public tbb::concurrent_hash_map<uint16, std::string>
{
public:
	typedef tbb::concurrent_hash_map<uint16, std::string> super;
	typedef super::const_accessor const_accessor;
	typedef super::value_type value_type;


	bool GetNetAddr(uint16 imsid, std::string& paddr)
	{
		const_accessor cacc;
		if (super::find(cacc, imsid))
		{
			paddr = cacc->second;
			return true;
		}
		return false;
	}

	bool InsertNetAddr(uint16 imsid, const std::string& net_addr)
	{
		return super::insert(value_type(imsid, net_addr));
	}
};

class CIMSMap : public tbb::concurrent_hash_map<uint16, boost::shared_ptr<CSImsHandler> >
{
public:
	typedef tbb::concurrent_hash_map<uint16, boost::shared_ptr<CSImsHandler> > super;
	typedef super::const_accessor const_accessor;
	typedef super::value_type value_type;

	bool GetHandler(uint16 imsid,boost::shared_ptr<CSImsHandler>& spHandler)
	{
		const_accessor cacc;
		if (super::find(cacc, imsid))
		{
			spHandler= cacc->second;
			return true;
		}
		return false;
	}

	bool InsertHandler(uint16 imsid,boost::shared_ptr<CSImsHandler>& spHandler)
	{
		return super::insert(value_type(imsid, spHandler));
	}

};

#endif /* IMS2SMAP_H_ */
