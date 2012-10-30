/*
 * Statistics.h
 *
 *  Created on: 2010-3-5
 *      Author: root
 */

#ifndef STATISTICS_MANAGER_H_
#define STATISTICS_MANAGER_H_

#include <ace/Singleton.h>
#include <ace/Null_Mutex.h>
#include <fstream>
#include <iostream>
#include <ace/Atomic_Op.h>
#include <ace/Date_Time.h>
#include "Configure.h"
#include "commandlib/include/cmdutil.h"
#include "netbaselib/include/packet/TcpOutPacket.h"
#include <map>
#include <vector>


// CStatistics class

class CStatisticsManager;
typedef ACE_Singleton<CStatisticsManager, ACE_Thread_Mutex> STATISTICS_MANAGER;


enum {
	ST_SVC_SS,
	ST_SVC_OMS,
	ST_SVC_GMS,
	ST_SVC_SMS,
	ST_SVC_UAP,
	ST_SVC_CS,
	ST_SVC_SIZE
};

class CStatisticsManager
{
public:
	CStatisticsManager() :
		m_bValid(false),
		m_packet_recv(0),
		m_packet_send(0),
		m_packet_tosend(0)
	{
	}

	virtual ~CStatisticsManager() {}

	class CStAsynTask
	{
	public:
		ACE_Atomic_Op<ACE_Thread_Mutex, int> m_atom_asyntasks_pending;	// ss等待中的异步请求数量
	};

	void Init()
	{
		if (CONFIGURE_MANAGER::instance()->m_debug_statistics)
			m_bValid = true;
		else
			m_bValid = false;

		memset(m_cmds_infos, 0, sizeof(m_cmds_infos));
	}

	uint64 GetRecvSum(uint16 cmd)
	{
		return m_cmds_infos[cmd].recv;
	}

	uint64 GetSendSum(uint16 cmd)
	{
		return m_cmds_infos[cmd].send;
	}

	uint64 GetCmdsTotalRecv()
	{
		return m_total_recv;
	}

	uint64 GetCmdsTotalSent()
	{
		return m_total_send;
	}

	uint64 GetPacketsSizeRecv()
	{
		return m_packet_recv;
	}

	uint64 GetPacketsSizeSent()
	{
		return m_packet_send;
	}

	void PutRecvWork(uint64 uin, uint16 cmd, size_t size)
	{
		if (m_bValid)
		{
			ACE_GUARD(ACE_Thread_Mutex, ace_mon, m_mutex_cmds);

			m_cmds_infos[cmd].recv++;
			m_total_recv++;

			m_packet_recv += size;
		}
	}

	void PutSendWork(uint64 uin, uint16 cmd, size_t size)
	{
		if (m_bValid)
		{
			ACE_GUARD(ACE_Thread_Mutex, ace_mon, m_mutex_cmds);

			m_cmds_infos[cmd].send++;
			m_total_send++;

			m_packet_send += size;
		}
	}

	void PutSendWork(const char* pdata, size_t size)
	{
		if (m_bValid)
		{
			uint64 uin = ntohl(*((uint32*)(pdata + offsetof(netbaselib::TCP_C2S_HEADER, qwUin))));
			uint16 cmd = ntohs(*((uint16*)(pdata + offsetof(netbaselib::TCP_C2S_HEADER, wCmd))));
			PutSendWork(uin, cmd, size);
		}
	}

	void PutToSendWork(uint64 uin, uint16 cmd, size_t size)
	{
		if (m_bValid)
		{
			ACE_GUARD(ACE_Thread_Mutex, ace_mon, m_mutex_cmds);

			m_cmds_infos[cmd].tosend++;
			m_total_tosend++;

			m_packet_tosend += size;
		}
	}

	void PutToSendWork(const char* pdata, size_t size)
	{
		if (m_bValid)
		{
			uint64 uin = ntohl(*((uint32*)(pdata + offsetof(netbaselib::TCP_C2S_HEADER, qwUin))));
			uint16 cmd = ntohs(*((uint16*)(pdata + offsetof(netbaselib::TCP_C2S_HEADER, wCmd))));
			PutToSendWork(uin, cmd, size);
		}
	}

	void Clear()
	{
		if (m_bValid)
		{
			ACE_GUARD(ACE_Thread_Mutex, ace_mon, m_mutex_cmds);

			memset(m_cmds_infos, 0, sizeof(m_cmds_infos));

			m_packet_recv = 0;
			m_packet_send = 0;
			m_packet_tosend = 0;
		}
	}

	void Clear(uint16 cmd)
	{
		if (m_bValid)
		{
			ACE_GUARD(ACE_Thread_Mutex, ace_mon, m_mutex_cmds);

			memset(&m_cmds_infos[cmd], 0, sizeof(CMD_INFO));
		}
	}

	// 开始异步请求
	void StartAsynTask(int svc)
	{
		++m_asyn_tasks[svc].m_atom_asyntasks_pending;
	}

	// 结束异步请求
	void FinishAsynTask(int svc)
	{
		--m_asyn_tasks[svc].m_atom_asyntasks_pending;
	}

	int GetAsynTaskPending(int svc)
	{
		return m_asyn_tasks[svc].m_atom_asyntasks_pending.value();
	}

	std::string ReportToFile()
	{
		if (m_bValid)
		{
			ACE_Date_Time logtime;
			char filename[256];
			ACE_OS::sprintf(filename, "report_%04d%02d%02d_%02d.%02d.%02d.txt", logtime.year(), logtime.month(), logtime.day(), logtime.hour(), logtime.minute(), logtime.second());

			ReportToFile(filename);

			return filename;
		}
		return "";
	}

	bool ReportToFile(const char* filename)
	{
		if (m_bValid)
		{
			std::ofstream ofs(filename);
			if (!ofs.bad())
			{
				Report(ofs);

				ofs.close();

				return true;
			}
		}
		return false;
	}

	bool Report(std::ostream& ostream, uint16 cmd)
	{
		if (m_bValid)
		{
			const char* cmdname = command_util::CommandName(cmd);

			ostream << (cmdname && cmdname[0] != '\0' ? cmdname : "unknown") << '(' << cmd << "): \t" << m_cmds_infos[cmd].recv << ", " << m_cmds_infos[cmd].send << "(" << m_cmds_infos[cmd].tosend << ")" << std::endl;
			return true;
		}
		return false;
	}

	bool Report(std::ostream& ostream);


private:
	bool m_bValid;

	// client commands

	struct CMD_INFO
	{
		CMD_INFO() { recv = 0; send = 0; tosend = 0; }
		uint64 recv;
		uint64 send;
		uint64 tosend;
	};

	CMD_INFO m_cmds_infos[0xFFFF];

	uint64 m_total_recv;
	uint64 m_total_send;
	uint64 m_total_tosend;

	uint64 m_packet_recv;
	uint64 m_packet_send;
	uint64 m_packet_tosend;

	ACE_Thread_Mutex m_mutex_cmds;

	// asyn tasks
	CStAsynTask m_asyn_tasks[ST_SVC_SIZE];
};




class CAsynTask_Track_SS
{
public:
	CAsynTask_Track_SS()
	{
		STATISTICS_MANAGER::instance()->StartAsynTask(ST_SVC_SS);
	}

	~CAsynTask_Track_SS()
	{
		STATISTICS_MANAGER::instance()->FinishAsynTask(ST_SVC_SS);
	}
};

class CAsynTask_Track_GMS
{
public:
	CAsynTask_Track_GMS()
	{
		STATISTICS_MANAGER::instance()->StartAsynTask(ST_SVC_GMS);
	}

	~CAsynTask_Track_GMS()
	{
		STATISTICS_MANAGER::instance()->FinishAsynTask(ST_SVC_GMS);
	}
};

class CAsynTask_Track_OMS
{
public:
	CAsynTask_Track_OMS()
	{
		STATISTICS_MANAGER::instance()->StartAsynTask(ST_SVC_OMS);
	}

	~CAsynTask_Track_OMS()
	{
		STATISTICS_MANAGER::instance()->FinishAsynTask(ST_SVC_OMS);
	}
};

class CAsynTask_Track_SMS
{
public:
	CAsynTask_Track_SMS()
	{
		STATISTICS_MANAGER::instance()->StartAsynTask(ST_SVC_SMS);
	}

	~CAsynTask_Track_SMS()
	{
		STATISTICS_MANAGER::instance()->FinishAsynTask(ST_SVC_SMS);
	}
};

class CAsynTask_Track_UAP
{
public:
	CAsynTask_Track_UAP()
	{
		STATISTICS_MANAGER::instance()->StartAsynTask(ST_SVC_UAP);
	}

	~CAsynTask_Track_UAP()
	{
		STATISTICS_MANAGER::instance()->FinishAsynTask(ST_SVC_UAP);
	}
};

class CAsynTask_Track_CS
{
public:
	CAsynTask_Track_CS()
	{
		STATISTICS_MANAGER::instance()->StartAsynTask(ST_SVC_CS);
	}

	~CAsynTask_Track_CS()
	{
		STATISTICS_MANAGER::instance()->FinishAsynTask(ST_SVC_CS);
	}
};

#endif /* STATISTICS_MANAGER_H_ */
