/*
 * CSMySqlConnMgr.h
 *
 *  Created on: 2011-8-4
 *      Author: root
 */

#ifndef CUSTOM_SERVICE_MYSQLCONNMGR_H_
#define CUSTOM_SERVICE_MYSQLCONNMGR_H_

//#include <mysql++/qparms.h>
//#include <mysql++/connection.h>
#include <mysql++/mysql++.h>
#include <boost/thread.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <deque>
#include <vector>
#include "type.h"
//#include "ICSDbMgr.h"


class MysqlDBServerInfo
{
public:
	MysqlDBServerInfo();

	int parse(char* parse_str);
	void set_bad();
	bool is_ok();

	std::string m_db;
	std::string m_server;
	std::string m_user;
	std::string m_pwd;
	bool m_ok;
	time_t m_reborn;
};

class MysqlConnPool
{
	typedef boost::mutex mutex_t;
	typedef mutex_t::scoped_lock scoped_lock_t;
	friend class MysqlConnPoolsRW;
	friend class CMySqlDbMgr;

	struct ConnInfo {
		ConnInfo(mysqlpp::Connection* c) :
			conn(c),
			last_used(time(0))
		{
		}

		mysqlpp::Connection* conn;
		time_t last_used;
	};

public:
	MysqlConnPool();
	virtual ~MysqlConnPool();

public:
	mysqlpp::Connection* grab();
	mysqlpp::Connection* grab_and_ping();
	void release(mysqlpp::Connection* conn);

public:
	void add(const MysqlDBServerInfo& dbinfo) {
		m_db_svrs.push_back(dbinfo);
	}

	bool empty() const {
		return m_db_svrs.empty();
	}

private:
	mutex_t m_mutex;
	std::deque<ConnInfo> m_conn_queue;

	std::vector<MysqlDBServerInfo> m_db_svrs;
	size_t m_round_robin;
};


class ScopedConnection
{
public:
	ScopedConnection(boost::shared_ptr<MysqlConnPool> spPool, mysqlpp::Connection* conn) :
		m_wk_pool(spPool),
		m_conn(conn)
	{
	}

	~ScopedConnection()
	{
		if (m_conn)
		{
			boost::shared_ptr<MysqlConnPool> spPool = m_wk_pool.lock();
			if (spPool)
				spPool->release(m_conn);
			else
				delete m_conn;
		}
	}

	mysqlpp::Connection* get() const
	{
		return m_conn;
	}

	mysqlpp::Connection* operator->() const
	{
		return m_conn;
	}

private:
	boost::weak_ptr<MysqlConnPool> m_wk_pool;
	mysqlpp::Connection* m_conn;
};


class MysqlConnPoolsRW
{
public:
	MysqlConnPoolsRW() :
		m_r_pool(new MysqlConnPool),
		m_w_pool(new MysqlConnPool)
	{
	}

	boost::shared_ptr<MysqlConnPool> get_r_pool() {
		return m_r_pool;
	}

	boost::shared_ptr<MysqlConnPool> get_w_pool() {
		return m_w_pool;
	}

private:
	boost::shared_ptr<MysqlConnPool> m_r_pool;
	boost::shared_ptr<MysqlConnPool> m_w_pool;

	friend class CMySqlDbMgr;
	friend class CMySqlConnMgr;
};



class CMySqlConnMgr
{
public:

	CMySqlConnMgr(){};
	virtual ~CMySqlConnMgr(){};
	bool load(const std::string& prefix = "mysql");



protected:

	MysqlConnPoolsRW* get_rw_pools(uint64 key)
	{
		MysqlConnPoolsRW* rw = &m_conn_pools[key % m_conn_pools.size()];
		return m_conn_pools.empty() ? NULL : rw;
	};

private:
	boost::ptr_vector<MysqlConnPoolsRW> m_conn_pools;



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


};










#endif /* MSGDB_MYSQL_H_ */
