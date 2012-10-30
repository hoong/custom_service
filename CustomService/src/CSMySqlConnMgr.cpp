/*
 * CMySqlConnMgr.cpp
 *
 *  Created on: 2011-8-4
 *      Author: root
 */

#include <time.h>
#include "CSMySqlConnMgr.h"
#include "mysql/mysql.h"
#include "mysql++/mysql++.h"
#include "pref.h"
#include "ConfigureMgr.h"
#include "logger.h"
//#include "../config.h"

#define CONN_IDLE_TIME 		(5*60)
#define CONN_TIMEOUT 			5
#define SERVER_RECOVER_TIMEOUT (5*60)

/*
#define SCOPED_CONN_WRITE(_rwpoolsw) \
	if (!_rwpoolsw) return -1; \
	boost::shared_ptr<MysqlConnPool> _spPoolw = _rwpoolsw->get_w_pool(); \
	mysqlpp::Connection* _connw = _spPoolw->grab_and_ping(); \
	if (!_connw) return -1; \
	ScopedConnection sc(_spPoolw, _connw);

#define SCOPED_CONN_READ(_rwpoolsw) \
	if (!_rwpoolsr) return -1; \
	boost::shared_ptr<MysqlConnPool> _spPoolr = _rwpoolsr->get_r_pool(); \
	mysqlpp::Connection* _connr = _spPoolr->grab_and_ping(); \
	if (!_connr) { \
		_connr = _rwpoolsr->get_w_pool()->grab_and_ping(); \
		if (!_connr) return -1; \
	} \
	ScopedConnection sc(_spPoolr, _connr);

#define SCOPED_KEY_CONN_WRITE(sc,key)\
	MysqlConnPoolsRW* _rwpoolsw = get_rw_pools(key); \
	SCOPED_CONN_WRITE(_rwpoolsw)

#define SCOPED_KEY_CONN_READ(sc,key) \
	MysqlConnPoolsRW* _rwpoolsr = get_rw_pools(key); \
	SCOPED_CONN_READ(_rwpoolsr)


#define SCOPED_GROUP_CONN_WRITE(sc) \
	MysqlConnPoolsRW* _rwpoolsw = get_rw_pools(gkey); \
	SCOPED_CONN_WRITE(_rwpoolsw)

#define SCOPED_GROUP_CONN_READ(sc) \
	MysqlConnPoolsRW* _rwpoolsr = get_rw_pools(gkey); \
	SCOPED_CONN_READ(_rwpoolsr)

#define SCOPED_USER_CONN_WRITE(sc) \
	MysqlConnPoolsRW* _rwpoolsw = get_rw_pools(uid); \
	SCOPED_CONN_WRITE(_rwpoolsw)

#define SCOPED_USER_CONN_READ(sc) \
	MysqlConnPoolsRW* _rwpoolsr = get_rw_pools(uid); \
	SCOPED_CONN_READ(_rwpoolsr)
*/



MysqlDBServerInfo::MysqlDBServerInfo() :
	m_ok(true)
{
}

int MysqlDBServerInfo::parse(char* parse_str)
{
	if (!parse_str)
		return -1;

	char* s = strtok(parse_str, ",");
	if (s)
	{
		m_server = s;
		s = strtok(NULL, ",");
		if (s)
		{
			m_db = s;
			s = strtok(NULL, ",");
			if (s)
			{
				m_user = s;
				s = strtok(NULL, ",");
				if (s)
					m_pwd = s;
			}
			return 0;
		}
	}
	return -1;
}

void MysqlDBServerInfo::set_bad()
{
	m_ok = false;
	m_reborn = ::time(0) + SERVER_RECOVER_TIMEOUT;
}

bool MysqlDBServerInfo::is_ok()
{
	if (m_ok || m_reborn >= ::time(0))
	{
		m_ok = true;
		return true;
	}
	return false;
}


MysqlConnPool::MysqlConnPool() :
	m_round_robin(0)
{
}

MysqlConnPool::~MysqlConnPool()
{
	while (!m_conn_queue.empty())
	{
		delete m_conn_queue.front().conn;
		m_conn_queue.pop_front();
	}
}

mysqlpp::Connection* MysqlConnPool::grab()
{
	mysqlpp::Connection* conn = NULL;
	{
		scoped_lock_t lock(m_mutex);
		if (m_conn_queue.size() > 1)
		{
			conn = m_conn_queue.back().conn;
			m_conn_queue.pop_back();

		}
	}

	if (conn)
		return conn;

	if (m_db_svrs.empty())
		return NULL;

	for (size_t i=0; i<m_db_svrs.size(); ++i)
	{
		int index = (m_round_robin++) % m_db_svrs.size();
		if (m_db_svrs[index].is_ok())
		{
			conn = new (std::nothrow)mysqlpp::Connection;
			if (conn)
			{
				try
				{
					conn->set_option(new mysqlpp::ConnectTimeoutOption(CONN_TIMEOUT));
					// conn->set_option(new mysqlpp::ReconnectOption(true)); // must above MySQL 5.1.6
					if (conn->connect(
							m_db_svrs[index].m_db.c_str(),
							m_db_svrs[index].m_server.c_str(),
							m_db_svrs[index].m_user.c_str(),
							m_db_svrs[index].m_pwd.c_str()))
					{
						return conn;
					}
				}
				catch (const std::exception& e)
				{
					LOG(error) << e.what() << ENDL;
				}

				m_db_svrs[index].set_bad();

				delete conn;
			}
		}
	}
	return NULL;
}

mysqlpp::Connection* MysqlConnPool::grab_and_ping()
{
	mysqlpp::Connection* conn = NULL;
	while ((conn = grab()) && !conn->ping())
	{
		delete conn;
		conn = NULL;
	}
	return conn;
}

void MysqlConnPool::release(mysqlpp::Connection* conn)
{
	scoped_lock_t lock(m_mutex);
	time_t curtime = time(NULL);
	while (!m_conn_queue.empty())
	{
		if (curtime - m_conn_queue.front().last_used > CONN_IDLE_TIME)
		{
			delete m_conn_queue.front().conn;
			m_conn_queue.pop_front();
		}
		else
			break;
	}
	ConnInfo ci(conn);
	m_conn_queue.push_back(ci);
}

bool CMySqlConnMgr::load(const std::string& prefix)
{
	base::Pref pref(CONFIGURE_MGR::instance().m_db_conf_file.c_str(), base::PREF_TYPE_INI);

	std::string count_key = prefix+".count";

	int count = 0;
	//pref.get_value("mysql.count", count);
	pref.get_value(count_key.c_str(), count);
	if (!count)
		return false;

	m_conn_pools.resize(count);

	for (int index = 0; index < count; ++index)
	{
		char skey[256];
		sprintf(skey, "%s.server%d",prefix.c_str() ,index+1);

		std::string str;
		pref.get_value(skey, str);

		std::vector<std::string> vec;
		char* s = strtok((char*)str.c_str(), ";");
		while (s)
		{
			vec.push_back(s);
			s = strtok(NULL, ";");
		}

		for (size_t i=0; i<vec.size(); ++i)
		{
			MysqlDBServerInfo dbinfo;
			if (0 == dbinfo.parse((char*)vec[i].c_str()))
			{
				LOG(trace) << "db" << index+1 << "(w): " << vec[i] << "\n";
				m_conn_pools[index].get_w_pool()->add(dbinfo);
			}
		}

		if (m_conn_pools[index].get_w_pool()->empty())
		{
				LOG(error) << "Cannot load " << skey << ENDL;
				return false;
		}

		sprintf(skey, "%s.server%d_read",prefix.c_str(), index+1);
		std::string str_r;
		if (0 == pref.get_value(skey, str_r))
		{
			std::vector<std::string> vec;
			char* s = strtok((char*)str_r.c_str(), ";");
			while (s)
			{
				vec.push_back(s);
				s = strtok(NULL, ";");
			}

			for (size_t i=0; i<vec.size(); ++i)
			{
				MysqlDBServerInfo dbinfo;
				if (0 == dbinfo.parse((char*)vec[i].c_str()))
				{
					LOG(trace) << "db" << index+1 << "(r): " << vec[i] << "\n";
					m_conn_pools[index].get_r_pool()->add(dbinfo);
				}
			}
		}
	}

	return true;
}
















