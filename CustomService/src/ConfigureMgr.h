/*
 * ConfigureMgr.h
 *
 *  Created on: 2010-8-16
 *      Author: root
 */

#ifndef MSS_CONFIGUREMGR_H_
#define MSS_CONFIGUREMGR_H_

#include <boost/thread/detail/singleton.hpp>
#include <vector>
#include <string>
#include  "type.h"

class CConfigureMgr
{
public:
	CConfigureMgr();
	virtual ~CConfigureMgr();

	bool init(const char* conf_file);

public:
	uint16 m_listen_port;
	uint8 m_threadpool_size;	// 线程池线程数量

	//logger
	int m_log_level;
	std::string m_log_filename;
	size_t m_log_rotation_size;
	size_t m_log_rotation_num;
	std::string m_db_conf_file;

	bool		m_log_daemonize;

	/*
	// MONGO DB
	struct MONGODB_SERVERS
	{
		std::vector<std::string> db_writes;
		std::vector<std::string> db_reads;
	};
	std::vector<MONGODB_SERVERS> m_mongodb_servers;	// MONGO DB address

	// MNT
	bool 	m_mnt_enabled;

	// MCM
	bool		m_mcm_enabled;

	// LOG

	//Hypertable DB
	std::string m_ht_installdir;
	std::string m_ht_config;
	*/
};

typedef boost::detail::thread::singleton<CConfigureMgr> CONFIGURE_MGR;

#endif /* MSS_CONFIGUREMGR_H_ */
