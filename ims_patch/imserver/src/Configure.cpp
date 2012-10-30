/*
 * Configure.cpp
 *
 *  Created on: 2009-11-13
 *      Author: root
 */

#include "Configure.h"
#include <ace/Configuration_Import_Export.h>
#include <ace/INET_Addr.h>
#include "Log.h"
#include "server/MNTManager.h"


CConfigureManager::CConfigureManager()
:m_s2c_max_uses(10000),
m_s2c_thread_pool_count(8),
m_s2c_thread_pool_queue_size(16777216),
m_s2c_send_queue_size(65536),
m_s2c_heartbeat_interval(60),
m_s2c_a2a_queue_limit(256),
m_s2c_line(0),
m_s2c_server_ver(0),
m_ims2s_thread_pool_count(8),
m_ims2s_queue_size(16777216),
m_ims2s_request_timeout_ms(10000),
m_ims2s_heartbeat_interval(30),
m_ims2sId(0),
m_s2mxs_disabled(false),
m_s2mxs_send_queue_size(1048576),
m_s2unas_disabled(false),
m_db_disabled(false),
m_uap_disabled(false),
m_uap_config_file("uap.conf.json"),
m_oap_insidepassport(0),
m_oms_disabled(false),
m_oms_threadpool_count(8),
m_oms_threadpool_queue_size(16777216),
m_oms_request_timeout_ms(10000),
m_oms_send_queue_size(1048576),
m_oms_heartbeat_interval(30),
m_oms_msgs_limit(100),
m_mss_disabled(false),
m_mss_threadpool_count(8),
m_mss_threadpool_queue_size(16777216),
m_mss_request_timeout_ms(10000),
m_mss_send_queue_size(1048576),
m_mss_heartbeat_interval(30),
m_ss_disabled(false),
m_ss_queue_size(16777216),
m_ss_heartbeat_interval(60),
m_ss_permaxsess(8),
m_gms_disabled(false),
m_group_client_conf_file("gms_client.conf"),
m_gms_queue_size(16777216),
m_gms_permaxsess(8),
m_mnt_disabled(false),
m_mnt_regtype(SVR_TYPE_IMS),
m_sms_disabled(false),
m_sms_threadpool_count(8),
m_sms_threadpool_queue_size(16777216),
m_sms_request_timeout_ms(60000),
m_sms_send_queue_size(1048576),
m_sms_heartbeat_interval(30),
m_sms_msgs_limit(10),
m_mcm_disabled(false),
m_mcm_report_interval(10),
m_log_daemonized(false),
m_log_filename("ims_log"),
m_debug_no_uap_auth(false),
m_debug_no_uap_auth_from(0ULL),
m_debug_no_uap_auth_to(-1ULL),
m_debug_statistics(false),
m_debug_statistics_report_interval_sec(0)
{
}

CConfigureManager::~CConfigureManager()
{
}

bool CConfigureManager::GetInt(const ACE_Configuration_Section_Key& key,	const ACE_TCHAR* name, int& value)
{
	ACE_TString strvalue;
	if (m_configHeap.get_string_value(key, name, strvalue) == 0)
	{
		value = atoi(strvalue.c_str());
		return true;
	}
	return false;
}

bool CConfigureManager::Init(const char* conf_file)
{
	if (m_configHeap.open() != 0)
		return false;

	ACE_Ini_ImpExp iniImp(m_configHeap);
	if (iniImp.import_config(conf_file) != 0)
		return false;

	const ACE_Configuration_Section_Key& root = m_configHeap.root_section();
	ACE_Configuration_Section_Key sec;

	ACE_TString strvalue;
	int intvalue;

	// ########## 用户客户端 ##########
	if (m_configHeap.open_section(root, "s2client", 0, sec) == 0)
	{
		// # 公网IP和端口
		if (m_configHeap.get_string_value(sec, "ip", strvalue) == 0
				&& GetInt(sec, "port", intvalue))
		{
			m_s2c_addr.ip = inet_network(strvalue.c_str());
			m_s2c_addr.port = intvalue;
		}
		else
			return false;

		// # 最大连接数
		if (GetInt(sec, "max_users", intvalue))
			m_s2c_max_uses = intvalue;

		// # 处理线程数
		if (GetInt(sec, "thread_pool_count", intvalue))
			m_s2c_thread_pool_count = intvalue;

		// # 处理线程队列大小 (byte)
		if (GetInt(sec, "thread_pool_queue_size", intvalue))
			m_s2c_thread_pool_queue_size = intvalue;

		// # 每个连接的发送队列大小 (byte)
		if (GetInt(sec, "send_queue_size", intvalue))
			m_s2c_send_queue_size = intvalue;

		// # 心跳检测时间 (s)
		if (GetInt(sec, "heartbeat_interval", intvalue))
			m_s2c_heartbeat_interval = intvalue;

		// # 每个连接的A2A消息队列上限
		if (GetInt(sec, "a2a_queue_limit", intvalue))
			m_s2c_a2a_queue_limit = intvalue;

		// # 电信1,网通2,教育网3
		if (GetInt(sec, "line", intvalue))
			m_s2c_line = intvalue;

		if (GetInt(sec,"server_ver", intvalue))
			m_s2c_server_ver = intvalue;

		// # 流量控制阀值 “a秒内最多处理b个业务接收c字节流量”
		if (m_configHeap.get_string_value(sec, "trafficflow_thresholds", strvalue) == 0)
		{
			m_s2c_thresholds.clear();

			char strs[256];
			ACE_OS::strsncpy(strs, strvalue.c_str(), sizeof(strs));

			char* p = strs;
			while(p)
			{
				FLOW_THRESHOLDS ft;

				char* pdest = strchr(p, ';');
				if (pdest)
					*pdest = '\0';

				int	r = sscanf(p, "%lu,%lu,%lu", &ft.msec, &ft.traffics, &ft.netflow);

				if (r != 3)
					break;

				ft.msec *= 1000;
				m_s2c_thresholds.push_back(ft);

				if (!pdest)
					break;
				p = pdest+1;
			}
		}
	}
	else
		return false;

	// ########## IMS与IMS之间连接 ##########
	if (m_configHeap.open_section(root, "ims2s", 0, sec) == 0)
	{
		// # 当前IMS内网的IP和端口
		if (m_configHeap.get_string_value(sec, "ip", strvalue) == 0
				&& GetInt(sec, "port", intvalue))
		{
			m_ims2s_addr.ip = inet_network(strvalue.c_str());
			m_ims2s_addr.port = intvalue;
		}
		else
			return false;

		// # 处理线程数
		if (GetInt(sec, "thread_pool_count", intvalue))
			m_ims2s_thread_pool_count = intvalue;

		// # 队列大小 (byte)
		if (GetInt(sec, "queue_size", intvalue))
			m_ims2s_queue_size = intvalue;

		// # 每个请求超时时间 (ms)
		if (GetInt(sec, "request_timeout_ms", intvalue))
			m_ims2s_request_timeout_ms = intvalue;

		// # 心跳检测时间 (s)
		if (GetInt(sec, "heartbeat_interval", intvalue))
			m_ims2s_heartbeat_interval = intvalue;
	}
	else
		return false;

	// ########## S2MXS ##########
	if (m_configHeap.open_section(root, "s2mxs", 0, sec) == 0)
	{
		// # 端口for mxs
		if (GetInt(sec, "port", intvalue))
		{
			m_s2mxs_defaddr.ip = m_ims2s_addr.ip;
			m_s2mxs_defaddr.port = intvalue;
		}

		// # 每个连接的发送队列大小 (byte)
		if (GetInt(sec, "send_queue_size", intvalue))
			m_s2mxs_send_queue_size = intvalue;

		// # 是否禁用MXS
		if (GetInt(sec, "disabled", intvalue))
			m_s2mxs_disabled = (intvalue != 0);
	}

	// ########## S2UNAS ##########
	if (m_configHeap.open_section(root, "s2unas", 0, sec) == 0)
	{
		// # 端口for mxs
		if (GetInt(sec, "port", intvalue))
		{
			m_s2unas_defaddr.ip = m_ims2s_addr.ip;
			m_s2unas_defaddr.port = intvalue;
		}

		// # 是否禁用MXS
		if (GetInt(sec, "disabled", intvalue))
			m_s2unas_disabled = (intvalue != 0);
	}

	// ########## 数据库 ##########
	if (m_configHeap.open_section(root, "db", 0, sec) == 0)
	{
		// # 是否禁用数据库
		if (GetInt(sec, "disabled", intvalue))
			m_db_disabled = (intvalue != 0);
	}

	// ########## UAP配置 ##########
	if (m_configHeap.open_section(root, "uap", 0, sec) == 0)
	{
		// # UAP配置文件名
		if (m_configHeap.get_string_value(sec, "configure_file", strvalue) == 0)
			m_uap_config_file = strvalue.c_str();

		// # 是否禁用UAP
		if (GetInt(sec, "disabled", intvalue))
			m_uap_disabled = (intvalue != 0);

		if (m_configHeap.get_string_value(sec, "oap_server", strvalue) == 0)
			m_oap_http_server = strvalue.c_str();
		if (GetInt(sec, "inside_passport", intvalue))
			m_oap_insidepassport = intvalue;
	}

	// ########## 状态服务器 ##########
	if (m_configHeap.open_section(root, "ss", 0, sec) == 0)
	{
		// # 服务器IP和端口
		if (m_configHeap.get_string_value(sec, "ip", strvalue) == 0
				&& GetInt(sec, "port", intvalue))
		{
			m_ss_defaddr.ip = inet_network(strvalue.c_str());
			m_ss_defaddr.port = intvalue;
		}

		// # 队列大小 (byte)
		if (GetInt(sec, "queue_size", intvalue))
			m_ss_queue_size = intvalue;

		// # 心跳检测时间 (s)
		if (GetInt(sec, "heartbeat_interval", intvalue))
			m_ss_heartbeat_interval = intvalue;

		// # 每个服务器连接数
		if (GetInt(sec, "sess_max", intvalue))
			m_ss_permaxsess = intvalue;

		// # 是否禁用状态服务器
		if (GetInt(sec, "disabled", intvalue))
			m_ss_disabled = (intvalue != 0);
	}

	// ########## 离线服务器 ##########
	if (m_configHeap.open_section(root, "oms", 0, sec) == 0)
	{
		// # 服务器IP和端口
		if (m_configHeap.get_string_value(sec, "ip", strvalue) == 0
				&& GetInt(sec, "port", intvalue))
		{
			m_oms_defaddr.ip = inet_network(strvalue.c_str());
			m_oms_defaddr.port = intvalue;
		}

		// # 处理线程数
		if (GetInt(sec, "thread_pool_count", intvalue))
			m_oms_threadpool_count = intvalue;

		// # 处理线程队列大小 (byte)
		if (GetInt(sec, "thread_pool_queue_size", intvalue))
			m_oms_threadpool_queue_size = intvalue;

		// # 每个请求超时时间 (ms)
		if (GetInt(sec, "request_timeout_ms", intvalue))
			m_oms_request_timeout_ms = intvalue;

		// # 每个连接的发送队列大小 (byte)
		if (GetInt(sec, "send_queue_size", intvalue))
			m_oms_send_queue_size = intvalue;

		// #心跳检测时间 (s)
		if (GetInt(sec, "heartbeat_interval", intvalue))
			m_oms_heartbeat_interval = intvalue;

		// # 一次获取个人离线消息的最大数量
		if (GetInt(sec, "get_offline_msg_limit", intvalue))
			m_oms_msgs_limit = intvalue;

		// # 是否禁用离线服务器
		if (GetInt(sec, "disabled", intvalue))
			m_oms_disabled = (intvalue != 0);
	}

	// ########## 消息存储 ##########
	if (m_configHeap.open_section(root, "mss", 0, sec) == 0)
	{
		// # 服务器IP和端口
		if (m_configHeap.get_string_value(sec, "ip", strvalue) == 0
				&& GetInt(sec, "port", intvalue))
		{
			m_mss_defaddr.ip = inet_network(strvalue.c_str());
			m_mss_defaddr.port = intvalue;
		}

		// # 处理线程数
		if (GetInt(sec, "thread_pool_count", intvalue))
			m_mss_threadpool_count = intvalue;

		// # 处理线程队列大小 (byte)
		if (GetInt(sec, "thread_pool_queue_size", intvalue))
			m_mss_threadpool_queue_size = intvalue;

		// # 每个请求超时时间 (ms)
		if (GetInt(sec, "request_timeout_ms", intvalue))
			m_mss_request_timeout_ms = intvalue;

		// # 每个连接的发送队列大小 (byte)
		if (GetInt(sec, "send_queue_size", intvalue))
			m_mss_send_queue_size = intvalue;

		// #心跳检测时间 (s)
		if (GetInt(sec, "heartbeat_interval", intvalue))
			m_mss_heartbeat_interval = intvalue;

		// # 是否禁用
		if (GetInt(sec, "disabled", intvalue))
			m_mss_disabled = (intvalue != 0);
	}

	// ########## 群服务器 ##########
	if (m_configHeap.open_section(root, "gms", 0, sec) == 0)
	{
		if (m_configHeap.get_string_value(sec, "configure_file", strvalue) == 0)
			m_group_client_conf_file = strvalue.c_str();

		// # 服务器IP和端口
		if (m_configHeap.get_string_value(sec, "ip", strvalue) == 0
				&& GetInt(sec, "port", intvalue))
		{
			m_gms_defaddr.ip = inet_network(strvalue.c_str());
			m_gms_defaddr.port = intvalue;
		}

		// # 队列大小 (byte)
		if (GetInt(sec, "queue_size", intvalue))
			m_gms_queue_size = intvalue;

		// # 每个服务器连接数
		if (GetInt(sec, "sess_max", intvalue))
			m_gms_permaxsess = intvalue;

		// # 是否禁用群服务器
		if (GetInt(sec, "disabled", intvalue))
			m_gms_disabled = (intvalue != 0);
	}

	// ########## 负载监控服务器 ##########
	if (m_configHeap.open_section(root, "mnt", 0, sec) == 0)
	{
		// # 是否禁用负载监控服务器
		if (GetInt(sec, "disabled", intvalue))
			m_mnt_disabled = (intvalue != 0);

		// # 注册类型
		if (GetInt(sec, "type", intvalue))
			m_mnt_regtype = intvalue;
	}

	// ########## 系统消息服务器 ##########
	if (m_configHeap.open_section(root, "sms", 0, sec) == 0)
	{
		// # 服务器IP和端口
		if (m_configHeap.get_string_value(sec, "ip", strvalue) == 0
				&& GetInt(sec, "port", intvalue))
		{
			m_sms_defaddr.ip = inet_network(strvalue.c_str());
			m_sms_defaddr.port = intvalue;
		}

		// # 处理线程数
		if (GetInt(sec, "thread_pool_count", intvalue))
			m_sms_threadpool_count = intvalue;

		// # 处理线程队列大小 (byte)
		if (GetInt(sec, "thread_pool_queue_size", intvalue))
			m_sms_threadpool_queue_size = intvalue;

		// # 每个请求超时时间 (ms)
		if (GetInt(sec, "request_timeout_ms", intvalue))
			m_sms_request_timeout_ms = intvalue;

		// # 每个连接的发送队列大小 (byte)
		if (GetInt(sec, "send_queue_size", intvalue))
			m_sms_send_queue_size = intvalue;

		// #心跳检测时间 (s)
		if (GetInt(sec, "heartbeat_interval", intvalue))
			m_sms_heartbeat_interval = intvalue;

		// # 一次获取系统消息的最大数量
		if (GetInt(sec, "get_system_msg_limit", intvalue))
			m_oms_msgs_limit = intvalue;

		// # 是否禁用离线服务器
		if (GetInt(sec, "disabled", intvalue))
			m_sms_disabled = (intvalue != 0);
	}

	// ########## 监控和控制服务器 ##########
	if (m_configHeap.open_section(root, "mcm", 0, sec) == 0)
	{
		// #是否禁用监控和控制服务器
		if (GetInt(sec, "disabled", intvalue))
			m_mcm_disabled = (intvalue != 0);

		// #汇报间隔时间 (s)
		if (GetInt(sec, "report_interval", intvalue))
			m_mcm_report_interval = intvalue;
	}

	// ########## 日志 ##########
	if (m_configHeap.open_section(root, "logger", 0, sec) == 0)
	{
		// # 是否启用daemon
		if (m_configHeap.get_string_value(sec, "logmode", strvalue) == 0)
		{
			m_log_daemonized = (strvalue != "console");
		}

		// # 日志文件名称
		if (m_configHeap.get_string_value(sec, "logfile", strvalue) == 0)
			m_log_filename = strvalue.c_str();
	}

	// ########## 调试 ##########
	if (m_configHeap.open_section(root, "debug", 0, sec) == 0)
	{
		// # 是否取消UAP认证，1代表是
		if (GetInt(sec, "debug_no_uap_auth", intvalue))
			m_debug_no_uap_auth = (intvalue != 0);
		if (GetInt(sec, "debug_no_uap_auth_from", intvalue))
			m_debug_no_uap_auth_from = intvalue;
		if (GetInt(sec, "debug_no_uap_auth_to", intvalue) && intvalue != 0)
			m_debug_no_uap_auth_to = intvalue;

		// # 是否使用统计和计数器，1代表是
		if (GetInt(sec, "debug_use_statistics", intvalue))
			m_debug_statistics = (intvalue != 0);
		if (m_debug_statistics)
		{
			// # 统计计数器输出间隔 (s)
			if (GetInt(sec, "debug_statistics_report_interval_sec", intvalue))
				m_debug_statistics_report_interval_sec = intvalue;

			// # 统计计数器输出到文件
			if (m_configHeap.get_string_value(sec, "debug_statistics_report_file", strvalue) == 0)
				m_debug_statistics_report_file = strvalue.c_str();
		}

		// 堆栈输出文件
		if (m_configHeap.get_string_value(sec, "debug_stack_file", strvalue) == 0)
		{
			m_debug_stack_file = strvalue.c_str();;
		}

	}

	if (m_configHeap.open_section(root, "cs", 0, sec) == 0)
	{
		// # 当前IMS内网的IP和端口
		if (m_configHeap.get_string_value(sec, "ip", strvalue) == 0
				&& GetInt(sec, "port", intvalue))
		{
			m_cs_defaddr.ip = inet_network(strvalue.c_str());
			m_cs_defaddr.port = intvalue;
		}
		else
			return false;


		if (GetInt(sec, "thread_pool_count", intvalue))
			m_cs_threadpool_count = intvalue;

		if (GetInt(sec, "request_timeout_ms", intvalue))
			m_cs_request_timeout_ms= intvalue;

		if (GetInt(sec, "heart_check_times", intvalue))
			m_cs_heart_check_times = intvalue;

		if (GetInt(sec, "heartbeat_interval", intvalue))
			m_cs_heartbeat_interval = intvalue;

	}



	return true;
}
