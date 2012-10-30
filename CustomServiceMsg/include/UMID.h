/*
 * MultPtIndexMap.h
 *
 *  Created on: 2010-1-27
 *      Author: root
 *
 *	Copied from imserver 2012-3-1
 *      多点登陆序号
 */

#ifndef UMID_H_
#define UMID_H_

#include <endian.h>
/*
#include "RefMap.h"
#include "Configure.h"
*/
#include <ext/hash_fun.h>
#include "packet/out_packet.h"
#include "packet/in_packet.h"

#define MULTPT_MAX 256 // 多点最大数量

#pragma pack(1)
// 多点结构
struct UMID
{
	UMID() : uin(0ULL) { clear(); }
	UMID(uint64 _uin) : uin(_uin) { clear(); }
	UMID(uint64 _uin,uint64 _multpt) : uin(_uin),multptvalue(_multpt) {}

	uint64 uin; // 用户uin

	union {
		struct {
#if 	__BYTE_ORDER == __LITTLE_ENDIAN
			uint64 multptindex:48; // 多点序号
			uint64 imsid:16; // 所在的服务器(从1开始),
#else
			uint64 imsid:16; // 所在的服务器(从1开始),
			uint64 multptindex:48; // 多点序号
#endif
		} multptinfo;

		uint64 multptvalue;
	};

	void Set(uint64 uin_, uint64 multptindex_, uint16 imsid_) { uin = uin_; multptinfo.multptindex = multptindex_; multptinfo.imsid = imsid_; }
	//void Set(uint64 uin_, uint64 multptindex_) { uin = uin_; multptinfo.multptindex = multptindex_; multptinfo.imsid = CONFIGURE_MANAGER::instance()->m_ims2sId; }
	void SetValue(uint64 uin_, uint64 value_) { uin = uin_; multptvalue = value_; }

	void clear() { multptvalue = 0; }
	bool isempty() const { return multptvalue == 0;}

	bool operator == (const UMID& t) const
	{
		return (uin == t.uin && multptvalue == t.multptvalue);
	}

	bool operator != (const UMID& t) const
	{
		return (uin != t.uin || multptvalue != t.multptvalue);
	}

	// for stl hash map
	size_t hash() const
	{
		return (size_t)((multptvalue << 32) + uin);
	}

	// for stl map 按imsid排序
	bool operator < (const UMID& t) const
	{
		if (multptinfo.imsid == t.multptinfo.imsid)
		{
			if (uin == t.uin)
				return multptinfo.multptindex < t.multptinfo.multptindex;

			return uin < t.uin;
		}
		return multptinfo.imsid < t.multptinfo.imsid;
	}


	friend base::packet::OutPacket&
	operator << (base::packet::OutPacket& out,const UMID& um)
	{
		out <<um.uin <<um.multptvalue;
		return out;
	};

	friend base::packet::InPacket&
	operator >> (base::packet::InPacket& in,UMID& um)
	{
		in >> um.uin >> um.multptvalue;
		return in;
	};

};

#pragma pack()



#endif /* MULTPTINDEXMAP_H_ */
