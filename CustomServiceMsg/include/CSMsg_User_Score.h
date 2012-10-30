#ifndef CSMSG_USER_SCORE_H_
#define CSMSG_USER_SCORE_H_

#include "type.h"

struct CSMsg_Request_User_Score
{
	CSMsg_Request_User_Score():
		m_nqueue(0),m_ncvsid(0),m_nscore(-1)
	{
		memset(&m_user_mid,0,sizeof(UMID));
	}

	CSMsg_Request_User_Score(const uint32& nQueueid, const uint64& cvs_id,const uint64& nUserUid, const uint64& nUserMultpt, const int8& nScore):
		m_nqueue(nQueueid),m_ncvsid(cvs_id),m_nscore(nScore)
	{
		m_user_mid.uin = nUserUid;
		m_user_mid.multptvalue = nUserMultpt;
	}
		
	template <typename IN>
	void decode(IN& inPacket)
	{
		inPacket >>this->m_nqueue>> this->m_user_mid>>this->m_ncvsid >>this->m_nscore;
	}

	template <typename OUT>
	void encode(OUT& outPacket)
	{
		outPacket <<this->m_nqueue<< this->m_user_mid<<this->m_ncvsid <<this->m_nscore;
	}

	UMID m_user_mid;
	uint32 m_nqueue;
	uint64 m_ncvsid;
	int8 m_nscore;
};


struct CSMsg_Response_User_Score
{
	CSMsg_Response_User_Score():m_nqueue(0)
	{
	}
	
	CSMsg_Response_User_Score(uint32 queue_id):m_nqueue(queue_id)
	{
	}

	template<typename IN>
	void decode(IN& inPacket)
	{
		inPacket >> m_nqueue;
	}

	template<typename OUT>
	void encode(OUT& outPacket)
	{
		outPacket<< m_nqueue;
	}

	uint32 m_nqueue;

};




#endif
