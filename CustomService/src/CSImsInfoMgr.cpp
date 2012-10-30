#include "CSImsInfoMgr.h"


int CSImsInfoMgr::init()
{
	if (!load(std::string("imsdb")))
	{
		return -1;
	}

	return load_allims();

}

int CSImsInfoMgr::load_allims()
{
	SCOPED_KEY_CONN_READ(sc,1);

	const std::string sQuery = "select id ,address from tbserver";
	mysqlpp::SQLQueryParms params;

	try
	{
		mysqlpp::Query query = sc->query(sQuery);
		query.parse();

		mysqlpp::StoreQueryResult result = query.store(params);
		if (!result)
			return -1;

		LOG(trace) << "query ims result: " << result << " row count: " << result.num_rows();

		if (result.num_rows() >0)
		{
			for (size_t i = 0;i<result.num_rows();i++)
			{
				mysqlpp::Row& row = result[i];
				uint16 imsid = row["id"];
				std::string addr;
				addr.assign(row["address"].data(),row["address"].size());
				m_imsmap.InsertNetAddr(imsid,addr);
			}
			return 0;
		}

	}
	catch (const std::exception& e)
	{
		LOG(error) << e.what() << ENDL;
	}
	return -1;

}


int CSImsInfoMgr::get_ims(const uint16& imsid,std::string& imsaddr)
{
	if (m_imsmap.GetNetAddr(imsid,imsaddr))
	{
		return 0;
	}

	return load_ims(imsid,imsaddr);
}

int CSImsInfoMgr::load_ims(const uint16& imsid,std::string& imsaddr)
{
	SCOPED_KEY_CONN_READ(sc,1);

	const std::string sQuery = "select address from tbserver where id = %0  ";
	mysqlpp::SQLQueryParms params;
	params << imsid;

	try
	{
		mysqlpp::Query query = sc->query(sQuery);
		query.parse();

		mysqlpp::StoreQueryResult result = query.store(params);
		if (!result)
			return -1;

		LOG(trace) << "query ims result: " << result << " row count: " << result.num_rows();

		if (result.num_rows() >0)
		{

			mysqlpp::Row& row = result[0];
			imsaddr.assign(row["address"].data(),row["address"].size());
			m_imsmap.InsertNetAddr(imsid,imsaddr);

			return 0;
		}

	}
	catch (const std::exception& e)
	{
		LOG(error) << e.what() << ENDL;
	}
	return -1;

}
