/*
 * ConfigureMgr.cpp
 *
 *  Created on: 2010-8-16
 *      Author: root
 */

#include "ConfigureMgr.h"
#include <boost/program_options.hpp>
#include <boost/program_options/detail/config_file.hpp>
#include "logger.h"
#include <fstream>


int getValue(std::map<std::string, std::string>& map, const char* key, int defvalue)
{
	return !map[key].empty() ? atoi(map[key].c_str()) : defvalue;
}


CConfigureMgr::CConfigureMgr()
{
}

CConfigureMgr::~CConfigureMgr()
{
}

bool CConfigureMgr::init(const char* conf_file)
{
	m_db_conf_file = conf_file;
	std::ifstream configfile;
	configfile.open(conf_file);
	if (!configfile.good())
		return false;

	std::set<std::string> options;
	options.insert("*");
	std::map<std::string, std::string> parameters;

	for (boost::program_options::detail::config_file_iterator itr(configfile, options), e; itr != e; ++itr)
	{
		parameters[itr->string_key] = itr->value[0];
	}

	// ########## MSS ##########

	/*
	// MSS地址
	if (!m_mss_addr.from(parameters["mss.address"].c_str()))
	{
		return false;
	}
	*/

	m_listen_port = getValue(parameters,"custom_service.port",2012);

	// 线程池线程数量
	m_threadpool_size = getValue(parameters, "custom_service.threadpool_size", 8);


	// 日志 
	m_log_level = getValue(parameters,"logger.level",0);
	m_log_rotation_size = getValue(parameters,"logger.rotation_size",10*1024*1024);
	m_log_rotation_num = getValue(parameters,"logger.rotation_num",10);
	m_log_filename = parameters["logger.filename"];

	/*

	// 超时天数
	m_mss_offlinemsg_expire_days = getValue(parameters, "mss.offlinemsg_expire_days", 7);
	m_mss_groupmsg_expire_days = getValue(parameters, "mss.groupmsg_expire_days", 7);
	m_mss_msg_expire_days = getValue(parameters, "mss.msg_expire_days", 7);


	// ########## MONGODB ##########

	// DB 数量
	int mssdbcount = getValue(parameters, "mongodb.dbcount", 0);

	for (int i=0; i<mssdbcount; ++i)
	{
		MONGODB_SERVERS ms;

		// MSS地址
		char addrs[256];
		sprintf(addrs, "mongodb.server.%u.write", i+1);

		std::string address_w = parameters[addrs];
		if (address_w.empty())
		{
			ND_LOG_ERROR("CConfigureMgr::init: %s empty.\n", addrs);
			break;
		}
		else
		{
			char* svr = ::strtok((char*)address_w.c_str(), ";");
			while (1)
			{
				if (svr)
				{
					ms.db_writes.push_back(svr);
					ND_LOG_INFO("mongodb server %s: %s.\n", addrs, svr);
				}
				else
					break;

				svr = ::strtok(NULL, ";");
			}
		}

		sprintf(addrs, "mongodb.server.%u.read", i+1);

		std::string address_r = parameters[addrs];
		if (address_r.empty())
		{
			ms.db_reads = ms.db_writes;
		}
		else
		{
			char* svr = ::strtok((char*)address_r.c_str(), ";");
			while (1)
			{
				if (svr)
				{
					ms.db_reads.push_back(svr);
					ND_LOG_INFO("mongodb server %s: %s.\n", addrs, svr);
				}
				else
					break;

				svr = ::strtok(NULL, ";");
			}
		}

		m_mongodb_servers.push_back(ms);
	}


	// ########## 负载监控服务器 ##########

	//  是否启用负载监控服务器
	m_mnt_enabled = getValue(parameters, "mnt.enabled", false);


	// ########## 监控和控制服务器 ##########

	//  是否启用监控和控制
	m_mcm_enabled = getValue(parameters, "mcm.enabled", false);
	*/


	// ########## LOG ##########
	m_log_daemonize = parameters["logger.logmode"] != "console";

	/*
	//HypertableDB
	m_ht_installdir = parameters["hypertable.installdir"];
	m_ht_config = parameters["hypertable.config"];
	*/


	return true;
}
