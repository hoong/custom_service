/*
 * UserProcess.cpp
 *
 *  Created on: 2009-10-16
 *      Author:
 */

#include "UserProcess.h"
#include "commandlib/include/cmd.h"
#include "MemPool.h"
#include "UserManager.h"
#include "uap/UAPManager.h"
#include "server/SSManager.h"
#include "statemachine/StateMachineLoginManager.h"
#include "ims2s/IMS2SManager.h"
#include "server/GroupManager.h"


//获取个人资料
//@输入：WY_GETUSR >> 帐号 >> dst.struDomain >> nMyUserInfo
//@处理：首先从UAP获取资料如果获取成功，继续向IMSDB获取扩展资料将二者结合返回给用户
//zhengnh 2009-10-26
int CUserProcess::OnGetUser(CAdaptedInPacket& in)
{
	ND_LOG_TRACE("CUserProcess::OnGetUser: uin=%Q:%Q.\n", GetUin(), GetMultPtValue());
	/*
	uint32 dwUin = 0;
	in >> dwUin;
	USER_INFO_EX userInfo;
	if (DB_MANAGER::instance()->GetUserInfoEx(dwUin,&userInfo)!= 0)
	{
		uint16 result = 400;

		netbaselib::CTcpS2COutPacket out;
		if (CreateS2CPacket(out, WY_GETUSR_EXT, in.GetSeq()))
		{
			out  << result << dwUin ;
			out.complete();
			SendPacketToClient(out);
		}
		return 0;
	}
	else
	{
		uint16 result = 200;

		netbaselib::CTcpS2COutPacket out;
		if (CreateS2CPacket(out, WY_GETUSR_EXT, in.GetSeq()))
		{
			out << result << dwUin;
			out << userInfo.points << userInfo.sendFrdGroupMsgPermission;
			out << userInfo.strCollege.c_str() << userInfo.strProf.c_str();
			out << "";//userInfo.strGameAccount.c_str() 返回给用户的帐号都改为空
			out << userInfo.picture;
			out << userInfo.strAddress.c_str();
			out << userInfo.strIntro.c_str();
			out.complete();
			SendPacketToClient(out);
		}
	}*/

	return 0;
}

//UAP获取个人资料回调函数
//＠输入：回调函数参数
//@处理：首先通过UAP获取用户的基本资料接着通过个人扩展资料表获取扩展资料
//zhengnh 2009-11-13
void CUserProcess::GetUserInfoCallBack(const uap::CallbackArgument& arg)
{
	ND_LOG_TRACE("CUserProcess::GetUserInfoCallBack.\n");

	/*if (arg.result == uap::UAP_OK && arg.response_code == 200)
	{
		USER_INFO userInfo;
		if(UAP_MANAGER::instance()->ProcessUserInfoJason(arg.response_body, userInfo))
		{
			//TODO 从本IMS数据库中获取扩展资料
			// test 模拟向客户端发送个人资料
			uint32  uin = (uint32)arg.user_data;
			USER_GUARD_EXIST(pUser, uin);
			if (pUser)
			{
				netbaselib::CTcpS2COutPacket out;
				pUser->CreateS2CPacket(out, WY_GETUSR, in.GetSeq());

				out << userInfo.dwUin << userInfo.strNickName.c_str();
				out<< userInfo.age << userInfo.sex;
				out << "" << userInfo.strProvince.c_str() << userInfo.strCity.c_str() << userInfo.strEmail.c_str();
				out << "" << "" << userInfo.strTelephone.c_str() << userInfo.strRealName.c_str() << userInfo.blood;
				out << "" << "" << "" << (uint8)0 << (uint8)0 << "" << "" << (uint8)0 << userInfo.strMobile.c_str();
				out << userInfo.strQq.c_str() << "" << uint8(0) << (uint32)1 << (uint8)0;
				out.complete();
				pUser->SendPacketToClient(out);
			}
		}
	}*/
}

//修改个人资料
//@输入：WY_REAUSR >> USER_INFO
//@处理：首先从UAP获取资料如果获取成功，继续向IMSDB获取扩展资料将二者结合返回给用户
//zhengnh 2009-10-26
int CUserProcess::OnReaUsr(CAdaptedInPacket& in)
{
	ND_LOG_TRACE("CUserProcess::OnReaUsr: uin=%Q:%Q.\n", GetUin(), GetMultPtValue());
/*
	USER_INFO_EX userInfo;
	uint8 IsChangePicture;
	in >> userInfo.points >> userInfo.sendFrdGroupMsgPermission >> userInfo.strCollege;
	in >> userInfo.strProf >> userInfo.strGameAccount >> userInfo.strAddress >> userInfo.picture;
	in >> IsChangePicture >> userInfo.strIntro;
	
	//add zhengnh 修改游戏帐号全改为空帐号
	userInfo.strGameAccount = "";

	uint16 result = 200;
	if (DB_MANAGER::instance()->UpdateUserInfoEx(GetUin(), userInfo)!= 0)
	{
		result = 400;
	}

	netbaselib::CTcpS2COutPacket out;
	if (CreateS2CPacket(out, WY_REAUSR, in.GetSeq()))
	{
		out << result;
		out.complete();
		SendPacketToClient(out);
	}

	if(result == 200)
	{
		if (IsChangePicture == 1)
		{
			// 通知好友头像改变
			netbaselib::CTcpS2COutPacket out2frd;
			out2frd << (uint16)1;
			out2frd << GetUin();
			out2frd << userInfo.picture;

			NotifyFriends(WY_SRV_MULTI_PIC, out2frd.getData(), out2frd.getDataLength());
		}
	}*/
	return 0;
}

//个人资料改变通知，包括头像，签名，心情等
int CUserProcess::OnNotifyPersonalInformation(CAdaptedInPacket& in)
{
	ND_LOG_TRACE("CUserProcess::OnNotifyPersonalInformation: uin=%Q:%Q. cmd =%d \n", GetUin(), GetMultPtValue(),in.GetCmd());

	netbaselib::CTcpS2COutPacket out2frd;
	out2frd << GetUin();
	out2frd.writeData((uint8*)in.getData(),in.getSize());

	NotifyFriends(in.GetCmd(), out2frd.getData(), out2frd.getDataLength());
	NotifyOwnMultPoints(in.GetCmd(), out2frd.getData(), out2frd.getDataLength());

	return 0;
}

//多点描述改变
int CUserProcess::OnMultPointDescChanged(CAdaptedInPacket& in)
{
	ND_LOG_TRACE("CUserProcess::OnMultPointDescChanged: uin=%Q:%Q.\n", GetUin(), GetMultPtValue());

	std::string strdesc;
	in >> strdesc;

	SetStatus(SS_UNDEFINE, NULL, SS_UNDEFINE, strdesc.c_str());

	// 向状态服务器记录状态描述改变
	SS_SESSION_GUARD(pSSClient, m_ss_session);
	if (pSSClient)
	{
		status_service_client::USER_STATUS userstatus;
		CreateSSUserStatus(userstatus);

		std::vector<status_service_client::USER_STATUS> vecUserStatus;
		vecUserStatus.push_back(userstatus);

		// 改变状态
		if (pSSClient->multispotOnlineUser((SS_CONTEXT_ID_TYPE)GetUin(), 2, vecUserStatus, CUserProcess::ChangeStatusCallBack, 0) < 0)
			ND_LOG_ERROR("CUserProcess::OnMultPointDescChanged:pSSClient->multispotOnlineUser fail!\n");
	}

	netbaselib::CTcpS2COutPacket out2frd;
	out2frd << strdesc;
	NotifyOwnMultPoints(WY_MULTPOINT_SRV_DESC_CHANGED, out2frd.getData(), out2frd.getDataLength());

	return 0;
}

//通知本人其他登陆点改变(如添加好友，重命名好友，添加好友组，重命名好友组，移动好友到其他组)
int CUserProcess::OnMultPointNotifySelfChanged(CAdaptedInPacket& in)
{
	ND_LOG_TRACE("CUserProcess::OnMultPointNotifySelfChanged: uin=%Q:%Q.\n", GetUin(), GetMultPtValue());

	NotifyOwnMultPoints(in.GetCmd(), (void*)in.getLeftData(), in.getLeftSize());

	return 0;
}

//登录准备完成
//@输入：
//@输出：
//zhengnh 2009-10-27
int CUserProcess::OnClientReady(CAdaptedInPacket& in)
{
	return 0;
}

//对用户发送的心跳包进行处理
//＠输入：WY_ACK
//＠处理：发送ACK给客户端
//zhengnh 2009-11-12
int CUserProcess::OnKeepAlive(CAdaptedInPacket& in)
{
/*	// 检查系统消息
	CheckSysMsgs();

	// 数据库记录群消息ackid
	if (m_groups_rec_max_ackids_intercount >= 0)
	{
		if (m_groups_rec_max_ackids_intercount >= GROUP_REC_MAX_ACKIDS_INTER)
		{
			RecordGroupMaxAckIdsToDB();
			m_groups_rec_max_ackids_intercount = -1;
		}
		else
			++m_groups_rec_max_ackids_intercount;
	}*/

	netbaselib::CTcpS2COutPacket out;
	if (CreateS2CPacket(out, WY_KEEPALIVE, in.GetSeq(), 200))
	{
		out.complete();
		SendPacketToClient(out);
	}

	return 0;
}

//对用户发送的时间同步包进行处理
//＠输入：WY_GET_WORKTIME
//＠处理：发送IMS时间给客户端
//zhengnh 2009-11-25
int CUserProcess::OnGetTime(CAdaptedInPacket& in)
{
	netbaselib::CTcpS2COutPacket out;
	if (CreateS2CPacket(out, WY_GET_WORKTIME, in.GetSeq()))
	{
		time_t curtime;
		ACE_OS::time(&curtime);
		out << (uint32)curtime;

		out.complete();

		SendPacketToClient(out);
	}
	
	return 0;
}

// 强迫自己某登陆点下线
int CUserProcess::OnMultPointForceOwnOffline(CAdaptedInPacket& in)
{
	USER_MULTPT_ID multptid;
	multptid.uin = GetUin();
	in >> multptid.multptvalue;

	ForceOwnMultPointOffline(multptid);

	return 0;
}

// 强迫自己其它所有登陆点下线
int CUserProcess::OnMultPointForceOwnAllOthersOffline(CAdaptedInPacket& in)
{
	ForceOwnOtherMultPointsOffline();

	return 0;
}

// 下线并清理
void CUserProcess::OfflineAndCleanup()
{
	ND_LOG_TRACE("CUserProcess::OfflineAndCleanup: uin=%Q:%Q.\n", GetUin(), GetMultPtValue());

	//记录所有最大群消息ID
//	RecordGroupMaxAckIdsToDB();
//	RecordSysMsgsMaxAckIdsToDB();

	SetStatus(SS_OFFLINE, "", 0, "");

	// 通知自己的其他登陆点下线
	NotifyOwnMultPointsStatusChange(WY_MULTPOINT_SRV_USER_OFFLINE);

	// 通知好友下线
	if (m_own_multpoints.size() == 0)
	{
		// 所有 登陆点都已经下线
		NotifyFriendsStatusChange(WY_SRV_USER_OFFLINE);
		//SSCloseUserAllAudioChating();
	}
	else
	{
		// 只是该登陆点下线
		NotifyFriendsStatusChange(WY_MULTPOINT_SRV_USER_OFFLINE);
		//关闭该登录点所有的语音聊天
		//SSCloseAllAudio();
	}

	// 结束所有聊天会话
	CloseAllChating();

	// 改变状态
	SS_SESSION_GUARD(pSSClient, m_ss_session);
	if (pSSClient)
	{
		status_service_client::USER_ID_WITH_MULTISEQ userid;
		userid.uid = GetUin();
		userid.multispotSeq = GetMultPtValue();

		std::vector<status_service_client::USER_ID_WITH_MULTISEQ> vec;
		vec.push_back(userid);

		// 状态服务器下线通知
		if (pSSClient->offlineUser((SS_CONTEXT_ID_TYPE)GetUin(), vec, CUserProcess::OfflineUserCallBack, 0) < 0)
			ND_LOG_ERROR("CSM_Login::OnStateLogouted: pSSClient->offlineUser Fail!\n");
	}

	//通知群成员离线
	NotifyAllGroupsOffline();
}

//static
// 用户退出时向SS注销的回调函数
void CUserProcess::OfflineUserCallBack(
	uint32_t dwReturnCode,	//返回码(参考SIP)
	SS_CONTEXT_ID_TYPE dwContextID,		//发起者标识
	void* initArgs,
	std::vector<status_service_client::USER_MULTISPOT_COUNT>& vecMultispotCount)	//当前用户多点个数
{
	ND_LOG_TRACE("CUserProcess::OfflineUserCallBack: returncode=%u.\n", dwReturnCode);
}


void CUserProcess::UpdateLoginInfo()
{
	netbaselib::NET_ADDR addr;
	if (GetNetHandle())
	{
		GetNetHandle()->GetAddr(addr);
		DB_MANAGER::instance()->RecordLoginInfo(m_multptid.uin, addr.tostring());
	};
}

//获取登陆信息
int CUserProcess::OnGetLoginInfo(CAdaptedInPacket& in)
{
	uint64 frduid;
	in >> frduid;

	std::string strAddr;
	time_t logintime;

	netbaselib::CTcpS2COutPacket out;

	if (0 == DB_MANAGER::instance()->GetLoginInfo(frduid, strAddr, logintime))
	{
		if (CreateS2CPacket(out, WY_GET_LOGININFO, in.GetSeq(), 200))
		{
			out << frduid << strAddr << (uint32)logintime << (uint32)time(NULL);
			out.complete();
			SendPacketToClient(out);
		}
	}
	else
	{
		if (CreateS2CPacket(out, WY_GET_LOGININFO, in.GetSeq(), 500))
		{
			out.complete();
			SendPacketToClient(out);
		}
	}
	return 0;
}

// 处理客户端发来的消息
int CUserProcess::OnPacketReceivedFromClient(CAdaptedInPacket& in)
{
	int ret = 0;
	uint16 cmd = in.GetCmd();
	uint16 cmdmask = cmd & 0xF000;

	if (cmdmask == WY_MULTPOIINT_MASK)
	{
		switch (cmd)
		{
		case WY_MULTPOINT_DESC_CHANGED: //多点描述改变
			ret = OnMultPointDescChanged(in);
			break;
		case WY_MULTPOINT_SUBSCRIBE_FRDS_ONLINE: // 获取多点在线好友列表
			ret = OnMultPointOnlineNotifyAndSubscribFrds(in);
			break;
		case WY_MULTPOINT_MSG_ACK: //多点消息接收确定
			ret = OnMultPointClientMsgAck(in);
			break;
		case WY_MULTPOINT_MSG_ACK_ONE:
			ret = OnMultPointClientMsgAckOne(in);
			break;
		case WY_MULTPOINT_MSG: //多点聊天
			ret = OnMultPointMsg(in);
			break;
		case WY_MULTPOINT_FORCE_OFFLINE: // 强迫某登陆点下线
			ret = OnMultPointForceOwnOffline(in);
			break;
		case WY_MULTPOINT_FORCE_ALLOTHERS_OFFLINE: // 强迫其它所有登陆点下线
			ret = OnMultPointForceOwnAllOthersOffline(in);
			break;
		default:
			if (cmd >= WY_MULTPOINT_NOTIFYSELF_START && cmd <= WY_MULTPOINT_NOTIFYSELF_END) // 转发通知自己的其他登录点
			{
				ret = OnMultPointNotifySelfChanged(in);
			}
		}
	}
	else if (cmdmask == WY_GROUP_MASK) // 群消息
	{
		group_cmd::UserPointID userptid;
		group_cmd::GroupKey gkey;
		in >> gkey.gid >> gkey.type;

		userptid.uid = GetUin();
		userptid.point_id = GetMultPtValue();

		base::packet::OutPacket out;

		if (cmd == group_cmd::CMD_C2N_LOGIN)
		{
			USER_STATUS_INFO us;
			GetStatus(us);
			out << us.szMultPtDesc << us.nStatus << us.szStatusDesc;
			out << ("PHPSESSID="+GetSessionID());
		}
		else if (cmd == group_cmd::CMD_C2N_JOIN
				|| cmd == group_cmd::CMD_C2N_APPROVE)
		{
			out.write_data(in.getLeftData(), in.getLeftSize());
			out << ("PHPSESSID="+GetSessionID());
		}
		else if (cmd == group_cmd::CMD_C2N_ACK_OUSIDER_MSG)
		{
			// 确认群成员通过im转发给群外人的消息
			return OnMultPointClientMsgAckOne(in);
		}
		else
			out.write_data(in.getLeftData(), in.getLeftSize());

		ret = GROUP_MANAGER::instance()->relay_msg(gkey, userptid, base::packet::Header(cmd, in.GetStatusCode(), in.GetSeq()), out);

	//	ret = OnRelayGroupMessage(in); // 转发群消息
	}
	else if (cmdmask == WY_SYSMSG_MASK) //系统消息
	{
/*		switch (cmd)
		{
		case WY_SYSMSG_INIT: // 初始化系统消息，发送系统消息查询需要的用户资料属性
			ret = OnSysMsgInit(in);
			break;
		case WY_SYSMSG_CREATE:	// 创建系统消息
			ret = OnSysMsgCreate(in);
			break;
		case WY_SYSMSG_GET_PERMESSION: // 获取创建系统消息权限
			ret = OnSysMsgGetPermession(in);
			break;
		case WY_SYSMSG_ACK:	// 应答IMS的消息
			ret = OnSysMsgAck(in);
			break;
		}*/
	}
	else if (cmdmask == WY_AUDIO_MASK) // 语音消息
	{
		/*
		switch (cmd)
		{
		case WY_AUDIO_CALL:					//主叫端向IMS发起语音请求
			ret = OnAudioCall(in);
			break;
		case WY_AUDIO_CALLCANCEL: //主叫端取消语音请求
			ret = OnAudioCallCancel(in);
			break;
		case WY_AUDIO_REFUSE:					//拒绝
			ret = OnAudioRefuse(in);
			break;
		case WY_AUDIO_ACCEPT:					//接受
			ret = OnAudioAccept(in);
			break;
		case WY_AUDIO_UPDATESTATUS:	//更新语音状态
			ret = OnAudioUpdateStatus(in);
			break;
		case WY_AUDIO_INVITE:					//发起语音邀请
			ret = OnAudioInvite(in);
			break;
		case WY_AUDIO_TERMINATE:			//结束语音聊天
			ret = OnAudioTerminate(in);
			break;
		case WY_GROUPTALK_AUDIO_INVITEONE://建立多人语音聊天室后邀请用户进入聊天室
			ret = OnGroupTalkAudioInviteOne(in);
			break;
		}*/
	}
	else if (cmdmask == WY_CS_MASK)
	{
		switch(cmd)
		{
			case WY_CS_AGENT_PRESENCE:
				ret = OnCSAgentPresence(in);
				break;
			case WY_CS_AGENT_ABSENCE:
				ret = OnCSAgentAbsence(in);
				break;
			case WY_CS_AGENT_ACCEPT:
				ret = OnCSAgentAccept(in);
				break;
			case WY_CS_AGENT_REFUSE:
				ret = OnCSAgentRefuse(in);
				break;
			case WY_CS_AGENT_REDIRECT:
				ret = OnCSAgentRedirect(in);
				break;
			case WY_CS_AGENT_GET_QUEUE:
				ret = OnCSAgentGetQueue(in);
				break;
			case WY_CS_AGENT_GET_QUEUEAGENT:
				ret = OnCSAgentGetQueueagent(in);
				break;
			case WY_CS_AGENT_USERAMOUNT:
				ret = OnCSAgentUserAmount(in);
				break;

			case WY_CS_USER_SCORE:
				ret = OnCSUserScore(in);
				break;
			case WY_CS_USER_JOIN:
				ret = OnCSUserJoin(in);
				break;
			case WY_CS_USER_DEPART:
				ret = OnCSUserDepart(in);
				break;

			case WY_CS_CONVERSATION_END:
				ret = OnCSConversationEnd(in);
				break;
			case WY_CS_SEND_MSG:
				ret = OnCSSendMsg(in);
				break;
			case WY_CS_GET_MSG:
				ret = OnCSGetMsg(in);
				break;

		}

	}
	else
	{
		switch (cmd)
		{
		case WY_MESSAGE:
			ret = OnMessage(in);
			break;

//		case WY_GET_CONFIG: //获取相关配置信息
//			ret = OnGetConfig(in);
//			break;
//		case WY_GET_NDHONOR_ME: //获取个人荣誉点
//			ret = OnGetNDHonorMe(in);
//			break;
//		case WY_GET_COMM_TEXT:
//			ret = OnGetCommText(in);
//			break;
//		case WY_GET_VISUALINFO: //获取形象秀的相关信息小照片,清晰照片的MD5
//			ret = OnGetVisualInfo(in);
//			break;
//		case WY_GETUSR_EXT: //获取个人扩展资料
//			ret = OnGetUser(in);
//			break;
//		case WY_REAUSR: //修改个人扩展资料
//			ret = OnReaUsr(in);
//			break;
		case WY_NICK_NOTIFY://用户昵称改变时通知
		case WY_MOOD_NOTIFY: //用户心情改变通知
		case WY_PICTURE_NOTIFY://用户头像改变通知
		case WY_PERSONAL_SIGN_NOTIFY:	//个人签名改变
		case WY_MDICNLVL_NOTIFY: // game unas notify
			ret = OnNotifyPersonalInformation(in);
			break;
		case WY_ADDFRD: //添加好友
			ret = OnAddFrd(in);
			break;
		case WY_REMFRD: //删除好友
			ret = OnRemFrd(in);
			break;
		case WY_SRV_MULTI_ONLINE: // 获取好友列表状态，同时上线通知好友
			ret = OnGetFrdsOnlineList(in);
			break;
/*		case WY_GET_PDEPT://获取优先组
			ret = OnGetPDept(in);
			break;
		case WY_SAVE_PDEPT://优先组操作
			ret = OnSavePDept(in);
			break;
		case WY_GET_CHANNEL://获取频道信息
			ret = OnGetChannel(in);
			break;
		case WY_NOTIFY_READY://登录准备完成
			ret = OnClientReady(in);
			break;*/
		case WY_CHANGE_STATUS://状态改变
			ret = OnChangeStatus(in);
	   break;
		case WY_SEARCH_ONLINE_IMS://查询所有用户在线
			ret = OnSearchOnlineUser(in);
			break;
		case WY_SEARCH_CONDITION_IMS://精确查找用户
			ret = OnSearchConditionUsers(in);
			break;
		case WY_GET_OFFLINEMSG://离线消息
			ret = OnGetOfflineMsg(in);
			break;
		case WY_OFFLINEMSG_ACK://客户端反馈已接收所有离线消息
			ret = OnOfflineMsgAck(in);
			break;
		case WY_OFFLINEMSG_ACK_ONE:
			ret = OnOfflineMsgAckOne(in);
			break;
		case WY_MSG://向用户转发普通消息
			ret = OnClientMsg(in);
			break;
		case WY_BROADCAST_MSG://向多个好友发送相同消息
			ret = OnBroadcastClientMsg(in);
			break;
		case WY_KEEPALIVE:
			ret = OnKeepAlive(in);
			break;
		case WY_GET_WORKTIME: //时间同步
			ret = OnGetTime(in);
			break;
		case WY_GET_LOGININFO: //获取登陆信息
			ret = OnGetLoginInfo(in);
			break;
		case WY_OL_NUM: //获取在线人数
			ret = OnGetOnlineNum(in);
			break;
/*	case WY_P2P_STREAM: //P2P传文件
		ret = OnP2PStream(in);
			break;
		case WY_P2P_PUNCHINGSUCCESS: // 客户端告诉服务器打洞成功
			ret = OnP2PPunchingSuccess(in);
			break;
 		case WY_GROUP_MSG_ACK://用户发送接收到的群消息ACK
			ret = OnGroupMsgAck(in);
			break;
		case WY_GET_GROUPS_RELATE_NUM://获取用户已经创建的群和加入的群数量
			ret = OnGetGroupsRelateNum(in);
			break;
		case WY_GROUP_AUTHJOINNUMBER://用户创建或解散群修改创建群和加入群数量
		case WY_GROUP_JOINNUMBER://用户加入群修改加入群数量
		case WY_GROUP_AUTHNUMBER://用户转移群修改创建群数量
			ret = OnUpdateGroupRelateNumber(in);
			break;
		case WY_GROUPMEMBER_AUTH://获取其他用户的创建群和加入群数量
			ret = OnGetFrdGroupRelateNum(in);
			break;
		case WY_GROUP_INIT: //群初始化
			ret = OnGroupInit(in);
			break;
		case WY_GROUP_TALK_INIT: //讨论组组初始化
			ret = OnGroupTalkInit(in);
			break;
*/
		default:// 无效命令处理
			{
				ND_LOG_WARNING("Invalid command[0x%X], uin=%Q.\n",cmd, GetUin());

				netbaselib::CTcpS2COutPacket out;
				if (CreateS2CPacket(out, WY_INVALID_COMMAND, in.GetSeq()))
				{
					out << cmd;
					out.complete();
					SendPacketToClient(out);
				}
				ret = 0;

			}
		}
	}

	return ret;
}


