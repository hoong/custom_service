/*
 * User.h
 *
 *  Created on: 2009-10-16
 *      Author: znh
 */

#ifndef USER_PROCESS_H_
#define USER_PROCESS_H_

#include "netbaselib/include/basictype.h"
#include "netbaselib/include/packet/TcpInPacket.h"
#include "netbaselib/include/packet/TcpOutPacket.h"
#include "MemPool.h"
#include "PacketProcess.h"
#include "MsgType.h"
#include "RefMap.h"
#include "MultPtIndexMap.h"
#include "server/SSManager.h"
#include "server/MSSManager.h"
//#include "server/SMSManager.h"
#include "server/GroupManager.h"
#include "uap/UAPManager.h"
#include "db/DBManager.h"
#include "ClientNetManager.h"
#include "StatisticsManager.h"
#include <set>


#define GROUP_REC_MAX_ACKIDS_INTER		10		// 将群最大应答id记录到数据库的间隔x心跳时间


// 请求上下文
struct REQUEST_CONTEXT
{
	USER_MULTPT_ID request_multptid; // 用户id
	uint32 request_seq; // 请求序号
};

struct SS_REQUEST_CONTEXT : REQUEST_CONTEXT, CAsynTask_Track_SS
{
};

struct GMS_REQUEST_CONTEXT : REQUEST_CONTEXT, CAsynTask_Track_GMS
{
};

struct OMS_REQUEST_CONTEXT : REQUEST_CONTEXT, CAsynTask_Track_OMS
{
};

struct SMS_REQUEST_CONTEXT : REQUEST_CONTEXT, CAsynTask_Track_SMS
{
};


struct CS_REQUEST_CONTEXT : REQUEST_CONTEXT, CAsynTask_Track_CS
{
	CS_REQUEST_CONTEXT(){};
	virtual ~CS_REQUEST_CONTEXT(){};
};


//用户信息及处理
class CUserProcess : public netbaselib::CRefThreadCounted<CUserProcess>
{
public:
	friend class CUserManager;
	friend class CSM_Login;

public:
	CUserProcess(uint64 uin, uint16 status, char* statusdesc, char* multptdesc, NET_HANDLE nethandle, uint32 sid)
	: m_spnetHandle(nethandle),
	m_sid(sid),
	m_inited(false),
	m_omsg_max_id(0)
	//m_groups_rec_max_ackids_intercount(-1)

	{
		uint64 mulptindex;
		ACE_OS::gettimeofday().to_usec(mulptindex);

		m_multptid.clear();
		m_multptid.Set(uin, mulptindex);

		SetStatus(status, statusdesc, 0, multptdesc);
	}

	virtual ~CUserProcess()
	{
	}

	// 获取用户id
	uint64 GetUin() const
	{
		return m_multptid.uin;
	}

	// 获取imsid+多点序号
	uint64 GetMultPtValue() const
	{
		return m_multptid.multptvalue;
	}

	const USER_MULTPT_ID& GetMultPtID() const
	{
		return m_multptid;
	}

	const NET_HANDLE GetNetHandle() const
	{
		return m_spnetHandle.Get();
	}

	uint32 GetSid() const
	{
		return m_sid;
	}

	// 检查uin和Net Handler是否相同
	bool Check(uint64 uin, const NET_HANDLE nethandle, uint32 sid) const
	{
		return (uin == m_multptid.uin && nethandle == GetNetHandle() && sid == m_sid);
	}

private:
	void Init()
	{
		// 绑定当前用户与SS服务器的连接会话
		SS_MANAGER::instance()->InitSession(&m_ss_session, m_multptid.uin);
	}

private:
	netbaselib::CScopedRefPtr<NET_HANDLE> m_spnetHandle; //连接句柄
	uint32 m_sid;
	bool m_inited;

	USER_MULTPT_ID m_multptid; //用户多点登陆帐号

public:
	SVR_SESSION m_ss_session; // 绑定SS会话

public:
	// 向客户端发送消息
	bool SendPacketToClient(netbaselib::CTcpS2COutPacket &out)
	{
		return CProtocolAdaptor::SendPacketToClient(GetNetHandle(), out);
	}

	// 向客户端发送消息
	bool SendPacketToClient(const void* data, size_t size)
	{
		return CProtocolAdaptor::SendPacketToClient(GetNetHandle(), (const char*)data, size);
	}

	// 根据消息命令创建发送到客户端的包
	bool CreateS2CPacket(netbaselib::CTcpS2COutPacket& packet, uint16 cmd, uint32 seq=0, uint16 status_code=0)
	{
		return CProtocolAdaptor::CreateS2CPacket(packet, m_multptid.uin, cmd, seq, GetSid(), status_code);
	}

public:
	// 处理客户端发来的消息
	int OnPacketReceivedFromClient(CAdaptedInPacket& in);


////////////////////
public:

	//获取个人资料
	int OnGetUser(CAdaptedInPacket& in);

	//UAP获取个人资料回调函数
	static void GetUserInfoCallBack(const uap::CallbackArgument& arg);

	//修改个人资料
	int OnReaUsr(CAdaptedInPacket& in);

	//个人资料改变通知，包括头像，签名，心情等
	int OnNotifyPersonalInformation(CAdaptedInPacket& in);

	//多点描述改变
	int OnMultPointDescChanged(CAdaptedInPacket& in);

	//通知本人其他登陆点改变(如添加好友，重命名好友，添加好友组，重命名好友组，移动好友到其他组)
	int OnMultPointNotifySelfChanged(CAdaptedInPacket& in);

	//登录准备完成
	int OnClientReady(CAdaptedInPacket& in);
	
	//处理用户发来的心跳包
	int OnKeepAlive(CAdaptedInPacket& in);

	//处理用户发来的同步时间
	int OnGetTime(CAdaptedInPacket& in);

	// 强迫自己某登陆点下线
	int OnMultPointForceOwnOffline(CAdaptedInPacket& in);

	// 强迫自己其它所有登陆点下线
	int OnMultPointForceOwnAllOthersOffline(CAdaptedInPacket& in);

	// 下线并清理
	void OfflineAndCleanup();

	static void OfflineUserCallBack(
		uint32_t dwReturnCode,	//返回码(参考SIP)
		SS_CONTEXT_ID_TYPE dwContextID,		//发起者标识
		void* initArgs,
		std::vector<status_service_client::USER_MULTISPOT_COUNT>& vecMultispotCount);	//当前用户多点个数

	//设置登录信息
	void UpdateLoginInfo();

	//获取登陆信息
	int OnGetLoginInfo(CAdaptedInPacket& in);


	// IMS2S ------------------------------------
public:
	// 来自其他好友的消息通知
	int OnIMS2SFriendNotify(const USER_MULTPT_ID& multptidfrom, uint16 cmd, uint8* pdata, uint32 datasize);

	// 来自其他好友的转发消息
	int OnIMS2SMsgRelay(const USER_MULTPT_ID& multptidfrom, uint16 cmd, uint8* pdata, uint32 size, uint64 ackid);


// Status ------------------------------------
public:
	// 本地保存和获取状态
	void SetStatus(uint16 status, const char* statusdesc, uint8 substatus, const char* multptdesc, USER_STATUS_INFO* poldstatusinfo=NULL);
	void GetStatus(USER_STATUS_INFO& statusinfo);

	// 生成状态服务器需要的用户状态信息
	void CreateSSUserStatus(status_service_client::USER_STATUS& ss_status);

	//用户状态改变
	int OnChangeStatus(CAdaptedInPacket& in);

	// 向SS修改状态改变回调函数
	static void ChangeStatusCallBack(
			uint32_t dwReturnCode,
			SS_CONTEXT_ID_TYPE dwContextID,
			void* initArgs,
			std::vector<status_service_client::USER_IMS_STATUS_S>& vecUserImsStatus);

	// 通知自己的其他登陆点
	bool NotifyOwnMultPoints(uint16 cmd, void* outdata=NULL, uint32 datasize=0);

	//用户状态改变时通知自己的其他登陆点
	bool NotifyOwnMultPointsStatusChange(uint16 cmd);

	//通知我的所有好友
	bool NotifyFriends(uint16 cmd, void* outdata, uint32 datasize);

	//用户状态改变时通知我的所有好友
	bool NotifyFriendsStatusChange(uint16 cmd);

	//用户状态改变时通知某个好友
	bool NotifyFriendStatusChange(uint16 cmd, const USER_MULTPT_ID& multptidfrd, IMS2S_MSG_NOTIFY_CALLBACK cb=NULL, IMS2S_CONTEXT_TYPE context=0);

	//用户状态改变时通知指定ims上的好友
	bool NotifyIMSFriendsStatusChange(uint16 cmd, uint16 imsid, const USER_MULTPT_ID* pmultptidfrds, size_t frdcount, IMS2S_MSG_NOTIFY_CALLBACK cb=NULL, IMS2S_CONTEXT_TYPE context=0);

	// 发送消息通知客户端改变好友状态
	bool SendFriendStatusChangeToClient(const USER_MULTPT_ID& multptidfrom, uint16 cmd, uint16 status, const std::string& statusdesc, uint8 substatus, const std::string& multptdesc);


// Friend ------------------------------------
public:
	//添加自己的其他登陆点
	void AddOwnMultPoint(const USER_MULTPT_ID& multptidfrom, uint8 substatus);

	//删除自己的其他登陆点
	void RemoveOwnMultPoint(const USER_MULTPT_ID& multptidfrom);

	// 更新自己的其他登陆点子状态
	void UpdateOwnMultPointSubStatus(const USER_MULTPT_ID& multptidfrom, uint8 substatus);

	//强制自己的某个登陆点下线
	void ForceOwnMultPointOffline(const USER_MULTPT_ID& multptid);

	//强制除了自己的其他登陆点下线
	void ForceOwnOtherMultPointsOffline();

	// 保存好友列表
	void CacheFriend(FRIEND_UNIT& frdunit);
	void CacheFriend(uint64 uinfrd, uint8 type=FRIEND_TYPE_UNDEFINE, uint16 status=SS_UNDEFINE, uint8 chating=CHATING_TYPE_UNDEFINE, uint8 subscribed_and_notified=SUBSCRIBE_TYPE_UNDEFINE, uint8 audio=VOST_UNDEFINE);

	// 获取缓存的好友
	bool GetCacheFriend(uint64 uinfrd, FRIEND_UNIT& frd_unit);

	// 更改好友列表中用户好友类型
	bool UpdateCacheFriendType(uint64 uinfrd, uint8 type);

	//更改好友列表中用户状态
	bool UpdateCacheFriendStatus(uint64 uinfrd, uint16 status);

	// 更改好友列表中订阅状态
	bool UpdateCacheFriendSubscribedAndNotified(uint64 uinfrd, uint8 subscribed_and_notified);

	//更改好友列表中用户聊天状态
	bool UpdateCacheFriendChatingStatus(uint64 uinfrd, uint8 chating);

	//更改好友列表中的语音聊天状态
	bool UpdateCacheFriendAudioStatus(uint64 uinfrd,uint8 audioStatus);

	//判断是否与好友在通话中
	bool IsAudioIng(uint64 uinfrd);

	//判断是否与好友已经在通话中
	bool IsAudioEd(uint64 uinfrd);

	//判断是否与好友的语音连接为空闲状态
	bool IsAudioFree(uint64 uinfrd);

	// 好友是否已经被订阅和上线通知过
	bool IsCacheFriendSubscribedAndNotifed(uint64 uinfrd);

	// 被好友订阅
	void SubscribedByFriend(const USER_MULTPT_ID& frd_multptid);

	// 好友取消订阅
	void UnsubscribedByFriend(const USER_MULTPT_ID& frd_multptid);

	// 获取多点在线好友列表
	int OnMultPointOnlineNotifyAndSubscribFrds(CAdaptedInPacket& in);

	// 获取好友列表状态，同时上线通知好友
	int OnGetFrdsOnlineList(CAdaptedInPacket& in);

	struct SS_GET_FRDS_ONLINE_CONTEXT : SS_REQUEST_CONTEXT
	{
		uint16 cmd;
		uint8 online_notify;
		uint8 need_multipt;
	};

	// 获取好友状态，如果notify_cmd != 0 上线通知好友
	bool GetFrdsStatusAndNotify(FRIEND_UNIT* frds, size_t frd_count, uint16 cmd, uint32 seq, uint8 online_notify, uint8 need_multpt);

	// 订阅对方状态回调
	static void SubscribeFrdCallBack(
			uint32_t dwReturnCode,	//返回码(参考SIP)
			SS_CONTEXT_ID_TYPE dwContextID,		//发起者标识
			void* initArgs);  //初始化指针，无初始化动作此参数可忽略

	//用户状态查询回调函数
	static void GetFrdsOnlineListCallBack(
			uint32 dwReturnCode,
			SS_CONTEXT_ID_TYPE dwContextID,
			void* initArgs,
			std::vector<status_service_client::USER_IMS_STATUS>& vecUserImsStatus); //用户IMS状态数组

	// 上线通知好友回调函数
	static void OnlineNotifyFrdsCallback(
		USER_MULTPT_ID					multptidreq,		// 发送方用户
		IMS2S_CONTEXT_TYPE		dwContext,				// Context ID
		uint16										dwReturnCode,		// 返回码
		USER_MULTPT_ID*				multptidsfails,
		uint16 										multptidsfailcount);

	// 添加好友
	int OnAddFrd(CAdaptedInPacket& in);

	// 删除好友
	int OnRemFrd(CAdaptedInPacket& in);


// Offline Message ------------------------------------
public:
	//获取离线消息
	int OnGetOfflineMsg(CAdaptedInPacket& in);

	//离线消息确认
	int OnOfflineMsgAck(CAdaptedInPacket& in);
	int OnOfflineMsgAckOne(CAdaptedInPacket& in);
	
	struct GET_OFFLINE_MSG_CONTEXT : OMS_REQUEST_CONTEXT
	{
		uint32 totalcount;
		uint64 lastackid;
	};

	//获取离线消息回调函数
	static void GetOfflineMsgCallBack(
			uint16								wReturnCode,		// 返回码
			OMS_CONTEXT_TYPE		dwContext,				// Context ID, if context != 200, the followings are invalid
			uint64								dwUin,						// 目标用户id
			RESPONSE_OMSG*			lpOMsgs,					// 离线记录数组
			uint32								dwOMsgCount,		// 记录条数
			uint64								qwNextFetchMsgIdStart,				// 下次获取的开始位置
			uint8									bIsOver);

	//删除OMS中存放该用户dwLastId之前的离线消息
	int DeleteOfflineMsg(uint64 qwLastId);
	int DeleteOfflineMsgOne(uint64 qwLastId);

	//删除离线消息回调函数 目前不作任何处理
	static void DeleteOfflineMsgCallBack(
			uint16						wReturnCode,		//返回码
			OMS_CONTEXT			dwContext);			// Context ID

	void SetOmsgMaxID(uint64 msgid)
	{
		if (msgid > m_omsg_max_id.value())
			m_omsg_max_id = msgid;
	}

	uint64 GetOmsgMaxID()
	{
		return m_omsg_max_id.value();
	}

	
// Search ------------------------------------
public:
	//搜索在线用户
	int OnSearchOnlineUser(CAdaptedInPacket& in);

	struct SS_SEARCHONLINE_CONTEXT : SS_REQUEST_CONTEXT
	{
		uint16 search_count;
	};

	//搜索在线用户回调函数
	static void SearchOnlineUserCallBack(
			uint32_t dwReturnCode,	//返回码(参考SIP)
			SS_CONTEXT_ID_TYPE dwContextID,		//发起者标识
			void* initArgs,
			std::vector<status_service_client::USER_IMS_STATUS_WITHOUT_MULTISPOT>& vecUserImsStatus); //用户IMS状态数组

	//精确查找用户状态
		int OnSearchConditionUsers(CAdaptedInPacket& in);

	//精确查找用户回调函数
	static void SearchConditionUsersCallBack(
			uint32_t dwReturnCode,	//返回码(参考SIP)
		 SS_CONTEXT_ID_TYPE dwContextID,		//发起者标识
		 void* initArgs,
		 std::vector<status_service_client::USER_IMS_STATUS>& vecUserImsStatus); //用户IMS状态数组

	//获取在线用户数量
	int OnGetOnlineNum(CAdaptedInPacket& in);

	//SS查询在线用户数量回调函数
	static void GetOnlineNumCallBack(
			uint32_t dwReturnCode,	//返回码(参考SIP)
			SS_CONTEXT_ID_TYPE dwContextID,		//发起者标识
			void* initArgs,  //初始化指针，无初始化动作此参数可忽略
			uint32_t dwCount);	// 用户数
						

// Relay Message ------------------------------------
public:
	//向用户转发普通消息
	int OnClientMsg(CAdaptedInPacket& in);

	//向多个好友发送相同消息
	int OnBroadcastClientMsg(CAdaptedInPacket& in);

	//多点用户消息确认
	int OnMultPointClientMsgAck(CAdaptedInPacket& in);
	int OnMultPointClientMsgAckOne(CAdaptedInPacket& in);

	//单点聊天
	int OnMultPointMsg(CAdaptedInPacket& in);

	//转发客户端消息到用户所有登录点
	bool RelayClientMsg(uint64 uindst, uint16 cmd, uint8* pdata, uint32 size, bool need_clientack_and_oms=false);

	//转发客户端消息到用户某个登录点
	bool RelayClientMsg(const USER_MULTPT_ID& multptidto, uint16 cmd, uint8* pdata, uint32 size, bool need_clientack_and_oms=false);

	//转发客户端聊天消息到用户所有登录点
	bool RelayClientMsg(uint8 type, const USER_MULTPT_ID& multptiddst, uint8* pmsgtext, uint32 textsize);

	// 结束所有聊天会话
	void CloseAllChating();

	// 聊天消息
	int OnMessage(CAdaptedInPacket& in);


// Group ------------------------------------
public:
	void set_group(const group_cmd::GroupKey& gkey)
	{
		ACE_GUARD(ACE_Thread_Mutex, ace_mon, m_mutex_groups);
		m_mygroups.insert(gkey);
	}

	void remove_group(const group_cmd::GroupKey& gkey)
	{
		ACE_GUARD(ACE_Thread_Mutex, ace_mon, m_mutex_groups);
		m_mygroups.erase(gkey);
	}

	// 状态改变通知所有群
	void NotifyAllGroupsStatusChanged(uint16 status, const char* statusdesc);

	//用户退出活掉线时通知其所有群离线
	void NotifyAllGroupsOffline();

/*
	// 转发群消息
	int OnRelayGroupMessage(CAdaptedInPacket& in);

	//处理用户发来的对接收到的群消息的ACK
	int OnGroupMsgAck(CAdaptedInPacket& in);

	// 初始化群列表，并从数据库读取所有群消息最大ID
	void InitGroup(const GROUP_KEY& gkey);

	// 添加群id到群列表
	void AddGroupId(uint32 groupId, uint8 type, uint64 ackid);

	// 获取群消息最大ID
	bool GetGroupMaxAckIds(uint32 groupId, uint8 gtype, uint64* pId);

	// 放入群消息最大ID
	bool PutGroupMaxAckIds(uint32 groupId, uint8 gtype, uint64 ackId);

	//用户退出时向DB写入该用户所有群的最大 MAXAckId
	void RecordGroupMaxAckIdsToDB();

	// 状态改变通知到指定群
	void  NotifyGroupStatusChanged(const GROUP_KEY& gkey, uint8 status, const char* statusdesc);

	// 状态改变通知所有群
	void NotifyAllGroupsStatusChanged(uint8 status, const char* statusdesc);

	//用户退出活掉线时通知其所有群离线
	void NotifyAllGroupsOffline();

	//从数据库获取创建的群数量
	int OnGetGroupsRelateNum(CAdaptedInPacket& in);

	//修改用户创建群，加入群数量
	int OnUpdateGroupRelateNumber(CAdaptedInPacket& in);

	//获取其他用户的创建群和加入群数量
	int OnGetFrdGroupRelateNum(CAdaptedInPacket& in);

	//todo 根据用户等级等一定算法获得用户可创建群的数量
	uint8 GetMaxCreateGroupsNum(uint32 dwUin = 0) const { return 20;}

	//todo 根据用户等级等一定算法获得用户可加入群的数量
	uint8 GetMaxHaveGroupsNum(uint32 dwUin = 0) const { return 40;}

	//群初始化
	int OnGroupInit(CAdaptedInPacket& in);

	//讨论组初始化
	int OnGroupTalkInit(CAdaptedInPacket& in);

	// 获取群离线消息
	bool GetGroupOfflineMsgs(const GROUP_KEY& gkey, uint16 cmd, uint32 reqseq, uint64 lastid, uint32 totalcount);

	// 讨论组
	struct GMS_CREATE_GROUP_TALK_CONTEXT : GMS_REQUEST_CONTEXT
	{
		int size;
		uint8 MsgData[MAX_S2C_PACKET_SIZE];
	};

	// 创建讨论组回调
	static void CreateGroupTalkCallback(
			uint16 dwReturnCode,	//返回码(参考SIP)
			void* dwContextID,		//发起者标识
			uint16 command,			//返回的命令类型
			uint32 packetContentSize,	//数据包体的长度
			void* packetContent);	// 接收的数据包体

	//获取讨论组回调
	static void GetGroupTalkCallback(
			uint16 dwReturnCode,	//返回码(参考SIP)
			GMS_CONTEXT_ID_TYPE dwContextID, //发起者标识
			uint16 command,			//返回的命令类型
			std::vector<group_message_service_client::IMS_INFO>* imsInfo_vec,//ims 信息：hostinfo 和 该ims下要转发的所有uid,hostname为空则为离线消息
			uint32 packetContentSize,	//数据包体的长度
			void* packetContent);	// 接收的数据包体
	*/
/*
// System message ------------------------------------
public:
	void SetSysMsgsStatus(int status)
	{
		m_sysmsgs_status = status;
	}

	int GetSysMsgsStatus()
	{
		return m_sysmsgs_status.value();
	}

	void AddSysMsgsIDs(const sms_msg::APP_TYPE& apptype, uint32 maxackid, uint32* smsgids, uint16 count);

	struct SMS_CHECK_CONTEXT : SMS_REQUEST_CONTEXT
	{
		sms_msg::APP_TYPE apptype;
		uint32 curappmaxid;
	};

	void RecordSysMsgsMaxAckIdsToDB();

	// 检查是否有新的符合条件的系统消息
	void CheckSysMsgs();

	//检查条件系统消息回调
	static void SysMsgCheckCallback(
		uint16								wReturnCode,		// 返回码
		SMS_CONTEXT_TYPE		dwContext,				// Context ID, if wReturnCode != 200, the followings are invalid
		uint32								dwMaxAckId,
		uint32*								lpFitMsgIDs,		// 符合条件的系统消息
		uint16								wFitCount);				// 记录条数

	// 初始化系统消息，发送系统消息查询需要的用户资料属性
	int OnSysMsgInit(CAdaptedInPacket& in);

	// 创建系统消息
	int OnSysMsgCreate(CAdaptedInPacket& in);

	// 创建系统消息回调函数
	static void SysMsgCreateCallback(
			uint16								wReturnCode,		// 返回码
			SMS_CONTEXT_TYPE		dwContext,				// Context ID
			uint32								dwAckID);

	//获取创建系统消息权限
	int OnSysMsgGetPermession(CAdaptedInPacket& in);

	// 应答IMS的消息
	int OnSysMsgAck(CAdaptedInPacket& in);

*/
/*
// P2P ---------------------------------
public:
	// P2P信令
	int OnP2PStream(CAdaptedInPacket& in);

	// 告诉服务器打洞成功
	int OnP2PPunchingSuccess(CAdaptedInPacket& in);

	int OnFileTransfer(CAdaptedInPacket& in);

// Audio Message ---------------------------------
public:
	//主叫端向IMS发起语音请求
	int OnAudioCall(CAdaptedInPacket& in);

	//主叫端取消语音请求
	int OnAudioCallCancel(CAdaptedInPacket& in);

	//拒绝
	int OnAudioRefuse(CAdaptedInPacket& in);

	//接受
	int OnAudioAccept(CAdaptedInPacket& in);

	//更新语音状态
	int OnAudioUpdateStatus(CAdaptedInPacket& in);

	//发起语音邀请
	int OnAudioInvite(CAdaptedInPacket& in);

	//结束语音聊天
	int OnAudioTerminate(CAdaptedInPacket& in);

	//建立多人语音聊天室后邀请用户进入聊天室
	int OnGroupTalkAudioInviteOne(CAdaptedInPacket& in);

	//SS中更新某语音聊天状态
	int SSCloseAudio(uint32 creater, uint32 partner, SS_DISCONNECT_P2P_AUDIO_CALLBACK callback);

	//SS中删除与该用户的所有语音聊天状态
	int SSCloseAllAudio();

	//当 用户所有登录点都下线时通知SS结束该用户所有的语音连接
	int SSCloseUserAllAudioChating();

	struct SS_AUDIO_CREATE_CONTEXT : SS_REQUEST_CONTEXT
	{
		uint32 partnerId;
	};

	//向SS注册新语音聊天连接回调函数
	static void UpdateAudioStatusConnectionCallback		(
			uint32_t dwReturnCode,	//返回码(参考SIP)
			SS_CONTEXT_ID_TYPE dwContextID,		//发起者标识
			void* initArgs,  //初始化指针，无初始化动作此参数可忽略
			std::vector<SS_USER_ID_TYPE>& vecUserId); //未能成功的被叫用户列表

	//向SS注册取消语音聊天连接回调函数
	static void UpdateAudioStatusClosenCallback(
			uint32_t dwReturnCode,	//返回码(参考SIP)
			SS_CONTEXT_ID_TYPE dwContextID,		//发起者标识
			void* initArgs,  //初始化指针，无初始化动作此参数可忽略
			std::vector<SS_USER_ID_TYPE>& vecUserId); //未能成功的被叫用户列表

	//Audio通知回调
	static void AudioNotifyCallback(
			USER_MULTPT_ID					multptidreq,		// 发送方用户
			IMS2S_CONTEXT_TYPE		dwContext,				// Context ID
			uint16										dwReturnCode,		// 返回码
			USER_MULTPT_ID*				multptidsfails,
			uint16 										multptidsfailcount);

*/

public:
	void SetSessionID(const char* sid)
	{
		ACE_WRITE_GUARD(ACE_RW_Thread_Mutex, ace_mon, m_mutex_status);
		m_session_id = sid ? sid : "";
	}

	std::string GetSessionID() {
		ACE_READ_GUARD_RETURN(ACE_RW_Thread_Mutex, ace_mon, m_mutex_status, "");
		return m_session_id;
	}
/////////////////////////////////
private:

// Status ----------------
private:
	ACE_RW_Thread_Mutex m_mutex_status;
	USER_STATUS_INFO m_user_status;
	std::string m_session_id;

// Language ----------------
private:
	uint32 m_nLangType; // TODO 用户语言类型


// Frds ----------------
private:
	typedef std::map<uint64 /*uin*/, FRIEND_UNIT, std::less<uint64>, netbaselib::ALLOCATOR<std::pair<const uint64, FRIEND_UNIT> > > MAP_FRD;
	MAP_FRD m_frds; // 好友信息map，key = uin

	typedef std::set<OWN_MULTPT_UNIT, std::less<OWN_MULTPT_UNIT>, netbaselib::ALLOCATOR<OWN_MULTPT_UNIT> > SET_OWN_MULTPTS;
	SET_OWN_MULTPTS m_own_multpoints; // 自己的其他登陆点

	typedef std::set<USER_MULTPT_ID, std::less<USER_MULTPT_ID>, netbaselib::ALLOCATOR<USER_MULTPT_ID> > SET_SUBSCRIBED;
	SET_SUBSCRIBED m_subscribed_by_frds; // 被好友订阅

	ACE_RW_Thread_Mutex m_mutex_frds;


// Offline Messages ----------------
private:
	ACE_Atomic_Op<ACE_Thread_Mutex, uint64> m_omsg_max_id;	// 个人离线消息最大id

// Group ----------------
private:
	ACE_Thread_Mutex m_mutex_groups;
	std::set<group_cmd::GroupKey> m_mygroups;

	/*
	struct GROUP_ACKID_INFO
	{
		uint64 lastackid;
		bool changed;
	};

	typedef std::map<GROUP_KEY, GROUP_ACKID_INFO, std::less<GROUP_KEY>, netbaselib::ALLOCATOR<std::pair<const GROUP_KEY, GROUP_ACKID_INFO> > > MAP_GROUP_ACKIDS;
	MAP_GROUP_ACKIDS m_groups_acked_ids;	// 各群已被确认的最大id
	ACE_RW_Thread_Mutex m_mutex_groups_ackedids;

	int m_groups_rec_max_ackids_intercount; // 将群最大应答id记录数据库间隔计数
*/


/*
// System Messages ----------------
private:
	class CSysMsg
	{
	public:
		CSysMsg(uint16 appid, uint16 subid, uint32 curmaxid)
		:	m_sysmsgs_maxid(curmaxid),
		m_sysmsg_update_ver(0)
		{
			m_sysmsgs_apptype.appinfo.appid = appid;
			m_sysmsgs_apptype.appinfo.subid = subid;
		}

		void SetMaxID(uint32 maxid)
		{
			if (m_sysmsgs_maxid < maxid)
				m_sysmsgs_maxid = maxid;
		}

		void AddMsg(uint32* smsgids, uint16 count);
		void SendMsgs(CUserProcess* pUser);
		bool Check(uint32* pappmaxid=NULL);
		void Ack(uint32 uin, uint32 ackid);

	public:
		typedef struct SMS_MSG_ID
		{
			sms_msg::APP_TYPE app;
			uint32 msgid;	// 消息id
		};

		sms_msg::APP_TYPE m_sysmsgs_apptype;	// 应用类型
		ACE_Atomic_Op<ACE_Thread_Mutex, uint32> m_sysmsgs_maxid;	// 当前最大系统消息id

		typedef std::deque<SMS_MSG_ID, netbaselib::ALLOCATOR<SMS_MSG_ID> > QUEUE_SYS_MSG_IDS;
		QUEUE_SYS_MSG_IDS m_sysmsgids;	// 符合条件但尚未发送的系统消息id

		typedef std::deque<uint32, netbaselib::ALLOCATOR<uint32> > QUEUE_SYS_MSG_TOACKS;
		QUEUE_SYS_MSG_TOACKS m_sysmsgtoacks;	// 待确认的消息id

		ACE_RW_Thread_Mutex m_mutex_smsgids;

		uint32 m_sysmsg_update_ver;
	};

	enum
	{
		USER_SMS_UNINITED = 0,
		USER_SMS_READY,
		USER_SMS_PROCESSING
	};
	ACE_Atomic_Op<ACE_Thread_Mutex, int> m_sysmsgs_status;

	std::vector<CSysMsg*, netbaselib::ALLOCATOR<CSysMsg*> > m_sysmsgs;
	sms_msg::MAP_PROPS m_sysmsgs_props;	// 用于系统消息的条件属性值
	ACE_RW_Thread_Mutex m_mutex_smsgs;

	*/


public:
	/*
	 * Custom_service
	 */
	int OnCSAgentPresence(CAdaptedInPacket& in);
	int OnCSAgentAbsence(CAdaptedInPacket& in);
	int OnCSAgentAccept(CAdaptedInPacket& in);
	int OnCSAgentRefuse(CAdaptedInPacket& in);
	int OnCSAgentRedirect(CAdaptedInPacket& in);
	int OnCSAgentGetQueue(CAdaptedInPacket& in);
	int OnCSAgentGetQueueagent(CAdaptedInPacket& in);
	int OnCSAgentUserAmount(CAdaptedInPacket& in);

	int OnCSUserJoin(CAdaptedInPacket& in);
	int OnCSUserDepart(CAdaptedInPacket& in);
	int OnCSUserScore(CAdaptedInPacket& in);

	int OnCSConversationEnd(CAdaptedInPacket& in);
	int OnCSSendMsg(CAdaptedInPacket& in);
	int OnCSGetMsg(CAdaptedInPacket& in);


};

#endif /* USER_PROCESS_H_ */
