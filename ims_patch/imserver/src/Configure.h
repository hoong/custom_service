/*
 * Configure.h
 *
 *  Created on: 2009-11-13
 *      Author: root
 */

#ifndef CONFIGURE_H_
#define CONFIGURE_H_

#include "netbaselib/include/basictype.h"
#include "netbaselib/include/NetAddr.h"
#include <ace/Null_Mutex.h>
#include <ace/Singleton.h>
#include <ace/Configuration.h>
#include <string>
#include <vector>


struct FLOW_THRESHOLDS
{
	ulong 	msec;
	ulong 	traffics;
	ulong	 	netflow;
};

// CConfigureManager class

class CConfigureManager;
typedef ACE_Singleton<CConfigureManager, ACE_Thread_Mutex> CONFIGURE_MANAGER;

class CConfigureManager
{
public:
	friend class ACE_Singleton<CConfigureManager, ACE_Thread_Mutex>;

	CConfigureManager();
	virtual ~CConfigureManager();

	bool Init(const char* conf_file);

private:
	bool GetInt(const ACE_Configuration_Section_Key& key,	const ACE_TCHAR* name, int& value);

public:
	ACE_Configuration_Heap m_configHeap;

public:
	// s2c
	netbaselib::NET_ADDR m_s2c_addr;
	uint32 m_s2c_max_uses;
	uint32 m_s2c_thread_pool_count;
	uint32 m_s2c_thread_pool_queue_size;
	uint32 m_s2c_send_queue_size;
	time_t m_s2c_heartbeat_interval;
	uint32 m_s2c_a2a_queue_limit;
	uint32 m_s2c_get_sms_limit;
	uint8 m_s2c_line; // 线路
	uint32 m_s2c_server_ver; // 服务版本

	// thresholds
	std::vector<FLOW_THRESHOLDS> m_s2c_thresholds;

	// ims2s
	netbaselib::NET_ADDR m_ims2s_addr;
	uint32 m_ims2s_thread_pool_count;
	uint32 m_ims2s_queue_size;
	uint32 m_ims2s_request_timeout_ms;
	time_t m_ims2s_heartbeat_interval;
	uint16 m_ims2sId; // 从数据库获取

	// s2mxs
	bool m_s2mxs_disabled;
	netbaselib::NET_ADDR m_s2mxs_defaddr;
	uint32 m_s2mxs_send_queue_size;

	// s2unas
	bool m_s2unas_disabled;
	netbaselib::NET_ADDR m_s2unas_defaddr;

	// db
	bool m_db_disabled;

	// uap
	bool m_uap_disabled;
	std::string m_uap_config_file;
	std::string m_oap_http_server;
	int m_oap_insidepassport;

	// oms
	bool m_oms_disabled;
	netbaselib::NET_ADDR m_oms_defaddr;
	uint32 m_oms_threadpool_count;
	uint32 m_oms_threadpool_queue_size;
	uint32 m_oms_request_timeout_ms;
	uint32 m_oms_send_queue_size;
	time_t m_oms_heartbeat_interval;
	uint32 m_oms_msgs_limit;

	// mss
	bool m_mss_disabled;
	netbaselib::NET_ADDR m_mss_defaddr;
	uint32 m_mss_threadpool_count;
	uint32 m_mss_threadpool_queue_size;
	uint32 m_mss_request_timeout_ms;
	uint32 m_mss_send_queue_size;
	time_t m_mss_heartbeat_interval;

	// ss
	bool m_ss_disabled;
	netbaselib::NET_ADDR m_ss_defaddr;
	uint32 m_ss_queue_size;
	uint32 m_ss_heartbeat_interval;
	uint32 m_ss_permaxsess;

	// gms
	bool m_gms_disabled;
	std::string m_group_client_conf_file;
	netbaselib::NET_ADDR m_gms_defaddr;
	uint32 m_gms_queue_size;
	uint32 m_gms_permaxsess;

	// mnt
	bool m_mnt_disabled;
	uint16 m_mnt_regtype; // ims在mnt注册的类型

	// sms
	bool m_sms_disabled;
	netbaselib::NET_ADDR m_sms_defaddr;
	uint32 m_sms_threadpool_count;
	uint32 m_sms_threadpool_queue_size;
	uint32 m_sms_request_timeout_ms;
	uint32 m_sms_send_queue_size;
	time_t m_sms_heartbeat_interval;
	uint32 m_sms_msgs_limit;

	// mcm
	bool m_mcm_disabled;
	uint32 m_mcm_report_interval;

	// log
	bool m_log_daemonized;
	std::string m_log_filename;

	// debug
	bool m_debug_no_uap_auth;
	uint64 m_debug_no_uap_auth_from;
	uint64 m_debug_no_uap_auth_to;
	bool m_debug_statistics;
	uint32 m_debug_statistics_report_interval_sec;
	std::string m_debug_statistics_report_file;
	std::string m_debug_stack_file;

	//cs
	netbaselib::NET_ADDR m_cs_defaddr;
	uint32 m_cs_threadpool_count;
	uint32 m_cs_request_timeout_ms;
	uint32 m_cs_heart_check_times;
	time_t m_cs_heartbeat_interval;

};


#endif /* CONFIGURE_H_ */
