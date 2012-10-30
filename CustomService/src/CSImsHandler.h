#ifndef CUSTOM_SERVICE_IMSHANDLER_H
#define CUSTOM_SERVICE_IMSHANDLER_H
#include <vector>
#include <map>

#include "net/handler.h"
#include "packet/header.h"
#include "CSMsg_Datatype.h"
#include "ImsCallbackQueue.h"
#include "ImsCallback.h"
#include "ImsCmd.h"

template <class T>
class listmap //:public std::map<uint16,std::vector<T> >
{
public:
	typedef std::vector<T> element_type ;
	typedef std::pair<uint16,std::vector<T> > value_type ;
	typedef typename std::map<uint16,std::vector<T> >::iterator listmap_it ;

	/*
#define element_type std::vector<T> 
#define value_type std::pair<uint16,std::vector<T> > 
#define listmap_it std::map<uint16,std::vector<T> >::iterator 
*/


	listmap()
	{
		m_it = m_map.begin();
	};
	~listmap()
	{
	};

	bool insert(const uint16& imsid,const T& element)
	{
		listmap_it it = m_map.find(imsid);
		if (it == m_map.end())
		{
			element_type elm;
			std::pair<listmap_it,bool> ret = m_map.insert(value_type(imsid,elm));
			it = ret.first;
		}
		it->second.push_back(element);
		m_it = m_map.begin();

		return true;

	};

	bool next(uint16& imsid,element_type*& list)
	{
		if (m_it == m_map.end())
		{
			return false;
		}
		imsid = m_it->first;
		list = &(m_it->second);
		m_it++;
		return true;
	};

private:
	std::map<uint16,std::vector<T> > m_map;
	//std::map<uint16,std::vector<T> >::iterator m_it;
	listmap_it m_it;

};

class CSImsHandler: public base::NetHandler<CSImsHandler,base::packet::Header>
{

public:
	virtual int handle_packet(base::packet::Header& header, base::packet::InPacket& in);
	void processResponse(base::packet::Header& header, boost::shared_ptr<base::packet::InPacket>& in);


	bool send_invite_list(const uint32& queue_id,const std::vector<INVITE_INFO>& invite_list);
	bool send_user_list(const uint32& queue_id,const std::vector<USER_INFO>& user_list);
	bool send_user_delcvs(const uint32& queue_id,const std::vector<USER_CONVERSATION>& usercvs_list);
	bool send_agent_delcvs(const uint32& queue_id,const uint64& cvs_id,const UMID& agent);
	/*
	bool send_user_newcvs(const uint32& queue_id,const uint64& cvs_id,const std::string &agent_name,const UMID& user);

	bool send_msg(IMS_CONTEXT_TYPE  rc,const MSG_INFO& msg,const UMID& recv);
	*/



private:
	CSImsCallbackQueue m_callback_queue;


};


#endif
