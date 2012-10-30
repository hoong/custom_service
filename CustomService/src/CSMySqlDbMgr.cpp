#include "CSMySqlDbMgr.h"
#include "logger.h"



namespace custom_service
{


bool CMySqlDbMgr::init()
{
	return load();
}

bool CMySqlDbMgr::verify_queue_agent(const uint32& queue_id,AGENT& agent)
{
	SCOPED_KEY_CONN_READ(sc,1);
	const std::string sQuery = "select b.name from queue_agent a,agent_info b where queue_id = %0 and a.agent_id = %1 and a.agent_id=b.agent_id";

	mysqlpp::SQLQueryParms params;
	params << queue_id << agent.id.uin;

	try
	{
		mysqlpp::Query query = sc->query(sQuery);
		query.parse();

		LOG(trace) << query.str(params) << ENDL;

		mysqlpp::StoreQueryResult result = query.store(params);
		if (!result)
			return false;

		LOG(trace) << "query msg result: " << result << " row count: " << result.num_rows() <<ENDL;

		if (result.num_rows() >0)
		{
			mysqlpp::Row& row = result[0];
			agent.name.assign(row["name"].data(),row["name"].size());
			return true;
		}

	}
	catch (const std::exception& e)
	{
		LOG(error) << e.what() << ENDL;
	}
	return false;
}

bool CMySqlDbMgr::get_queue_info(const uint32_t& queue_id,std::string& name)
{
	SCOPED_KEY_CONN_READ(sc,1);
	const std::string sQuery = "select name from queue_info where queue_id = %0 ";

	mysqlpp::SQLQueryParms params;
	params << queue_id ;

	try
	{
		mysqlpp::Query query = sc->query(sQuery);
		query.parse();

		LOG(trace) << query.str(params) << ENDL;

		mysqlpp::StoreQueryResult result = query.store(params);
		if (!result)
			return false;

		LOG(trace) << "query msg result: " << result << " row count: " << result.num_rows()<<ENDL;

		if (result.num_rows() >0)
		{
			mysqlpp::Row& row = result[0];
			name.assign(row["name"].data(),row["name"].size());
			return true;
		}

	}
	catch (const std::exception& e)
	{
		LOG(error) << e.what() << ENDL;
	}
	return false;

}


bool CMySqlDbMgr::get_queue_list(const uint64 agent_id,std::vector<QUEUE_INFO>& queue_list)
{
	SCOPED_KEY_CONN_READ(sc,1);
	const std::string sQuery = "select a.wg_id,a.name wg_name,b.queue_id,b.name queue_name \
				    from wg_info a, queue_info b,wg_queue c,queue_agent d \
		where d.agent_id = %0 and d.queue_id = b.queue_id and d.queue_id= c.queue_id and c.wg_id = a.wg_id\
		";
	mysqlpp::SQLQueryParms params;
	params << agent_id;


	try
	{
		mysqlpp::Query query = sc->query(sQuery);
		query.parse();

		LOG(trace) << query.str(params) << ENDL;

		mysqlpp::StoreQueryResult result = query.store(params);
		if (!result)
			return false;

		LOG(trace) << "query msg result: " << result << " row count: " << result.num_rows()<<ENDL;
		unsigned idx =0;
		for (; idx<result.num_rows();idx++)
		{
			mysqlpp::Row& row = result[idx];
			QUEUE_INFO item;
			item.wg_id = row["wg_id"];
			item.wg_name = row["wg_name"].data();
			item.queue_id = row["queue_id"];
			item.queue_name.assign(row["queue_name"].data(),row["queue_name"].size());
			queue_list.push_back(item);
		}

		return true;

	}
	catch (const std::exception& e)
	{
		LOG(error) << e.what() << ENDL;
	}
	return false;

}


bool CMySqlDbMgr::save_conversation_score(const uint32& queue_id,const uint64& cvs_id,const int8& cvs_score)
{
	SCOPED_KEY_CONN_WRITE(sc,1);
	const std::string sQuery = "update converesation set score = %0 where conversation_id = %1 and queue_id = %2";

	mysqlpp::SQLQueryParms params;
	params << cvs_score <<cvs_id <<queue_id;


	try
	{
		mysqlpp::Query query = sc->query(sQuery);
		query.parse();

		LOG(trace) << query.str(params) << ENDL;

		mysqlpp::SimpleResult result = query.execute(params);
		if (result)
			return true;
	}
	catch (const std::exception& e)
	{
		LOG(error) << e.what() << ENDL;
	}
	return false;

}


bool CMySqlDbMgr::save_conversation_info(CONVERSATION* cvs)
{
	SCOPED_KEY_CONN_WRITE(sc,1);
/*
	const std::string sQuery = "insert into conversation (conversation_id,queue_id,agent_id,user_id,user_ip,user_score)\
	       	values(%0,%1,%2,%3,%4,%5)";
*/

	const std::string sQuery = "insert into conversation (conversation_id,queue_id,agent_id,user_id,user_ip,user_score,create_date)\
	       	values(%0,%1,%2,%3,%4,%5,now())  on duplicate key update agent_id=%1 and user_score=%5 ";

	/*
INSERT INTO osc_space_visit_records(space,user,visit_count,ip,visit_time) VALUES(?,?,?,?,?) 
2	ON DUPLICATE KEY UPDATE visit_count=visit_count+1,ip=?,visit_time=NOW()
*/
	
	try
	{

	mysqlpp::SQLQueryParms params;
	params << cvs->cvs_id <<cvs->queue_id<<cvs->agent->id.uin<< cvs->user.id.uin<<cvs->user.address<< cvs->score;

		mysqlpp::Query query = sc->query(sQuery);
		query.parse();

		LOG(trace) << query.str(params) << ENDL;

		mysqlpp::SimpleResult result = query.execute(params);
		if (result)
			return true;
	}
	catch (const std::exception& e)
	{
		LOG(error) << e.what() << ENDL;
	}
	return false;

}



bool CMySqlDbMgr::last_cvsid(uint64& last)
{
	SCOPED_KEY_CONN_READ(sc,1);
	const std::string sQuery = "select ifnull(max(conversation_id),0) last from conversation ";
	last = 0;

	mysqlpp::SQLQueryParms params;

	try
	{
		mysqlpp::Query query = sc->query(sQuery);
		query.parse();

		LOG(trace) << query.str(params) << ENDL;

		mysqlpp::StoreQueryResult result = query.store(params);

		if (!result)
			return false;

		LOG(trace) << "query msg result: " << result << " row count: " << result.num_rows()<<ENDL;

		if (result.num_rows() >0)
		{
			mysqlpp::Row& row = result[0];
			last = row["last"];
			return true;
		}

	}
	catch (const std::exception& e)
	{
		LOG(error) << e.what() << ENDL;
	}
	return false;

}

bool CMySqlDbMgr::last_msgid(uint64& last)
{
	SCOPED_KEY_CONN_READ(sc,1);
	const std::string sQuery = "select ifnull(max(msg_id),0) last from msg";
	last = 0;

	mysqlpp::SQLQueryParms params;

	try
	{
		mysqlpp::Query query = sc->query(sQuery);
		query.parse();

		LOG(trace) << query.str(params) << ENDL;

		mysqlpp::StoreQueryResult result = query.store(params);

		if (!result)
			return false;

		LOG(trace) << "query msg result: " << result << " row count: " << result.num_rows() <<ENDL;

		if (result.num_rows() >0)
		{
			mysqlpp::Row& row = result[0];
			last = row["last"];
			return true;
		}

	}
	catch (const std::exception& e)
	{
		LOG(error) << e.what() << ENDL;
	}
	return false;

}


bool CMySqlDbMgr::save_msg(  const MSG_INFO&  msginfo)
{
	SCOPED_KEY_CONN_WRITE(sc,1);
	const std::string sQuery = "insert into msg (msg_id,cvs_id,agent,user,flag,time,text)\
	       	values(%0,%1,%2,%3,%4,now(),'%5')";


	try
	{
		mysqlpp::SQLQueryParms params;
		//params << cvs->cvs_id <<cvs->queue_id<<cvs->agent->id.uid<< cvs->user.id.uid<<cvs->user.address<< cvs->score;
		params <<msginfo.msg_id <<msginfo.cvs_id<<msginfo.agent<<msginfo.user
			<<msginfo.send_flag<<msginfo.send_text;


		mysqlpp::Query query = sc->query(sQuery);
		query.parse();

		LOG(trace) << query.str(params) << ENDL;

		mysqlpp::SimpleResult result = query.execute(params);
		if (result)
			return true;
	}
	catch (const std::exception& e)
	{
		LOG(error) << e.what() << ENDL;
	}
	return false;

}

bool CMySqlDbMgr::get_msg(const uint64& cvs_id,std::vector<MSG_INFO>& msg_list)
{
	SCOPED_KEY_CONN_READ(sc,1);
	const std::string sQuery = " select msg_id,cvs_id,agent,user,flag,unix_timestamp(time) time,text from msg where cvs_id = %0 order by msg_id ";
	mysqlpp::SQLQueryParms params;
	params << cvs_id;


	try
	{
		mysqlpp::Query query = sc->query(sQuery);
		query.parse();

		LOG(trace) << query.str(params) << ENDL;

		mysqlpp::StoreQueryResult result = query.store(params);
		if (!result)
			return false;

		LOG(trace) << "query msg result: " << result.num_rows()<< " row count: " << result.num_rows() <<ENDL;
		unsigned idx =0;
		for (; idx<result.num_rows();idx++)
		{
			mysqlpp::Row& row = result[idx];
			MSG_INFO item;
			item.msg_id= row["msg_id"];
			item.cvs_id= row["cvs_id"];
			item.agent= row["agent"];
			item.user= row["user"];
			item.send_flag= row["flag"];
			item.send_time= row["time"];
			item.send_text.assign(row["text"].data(),row["text"].size());
			msg_list.push_back(item);
		}

		return true;

	}
	catch (const std::exception& e)
	{
		LOG(error) << e.what() << ENDL;
	}
	return false;


}



}
