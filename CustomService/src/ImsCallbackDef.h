/*
 * ImsCallbackDef.h
*/
#ifndef IMS_CALLBACK_DEF_H
#define IMS_CALLBACK_DEF_H

#include <boost/shared_ptr.hpp>
#include "Queue.h"
#include "CSNetHandler.h"

#define SEQ_INDEX_NONE -1UL
#define SEQ_INDEX_MASK  0x7FFFFFFF
#define CALLBACK

//typedef void* IMS_CONTEXT_TYPE ;


struct i_ims_context
{
	i_ims_context(){};
	i_ims_context(const base::packet::Header& hdr,custom_service::QUEUE* q_ptr):header(hdr),queue_ptr(q_ptr){};
	virtual ~i_ims_context(){};

	base::packet::Header header;
	custom_service::QUEUE* queue_ptr;
	boost::shared_ptr<custom_service::CSNetHandler> handler;


};

typedef boost::shared_ptr<i_ims_context> IMS_CONTEXT_TYPE;

enum IMS_CALLBACK_TYPE
{
	IMS_CALLBACKTYPE_RELAY_INVITE_LIST,
	IMS_CALLBACKTYPE_RELAY_USER_LIST,
	IMS_CALLBACKTYPE_NOTIFY_CVSEND,
	IMS_CALLBACKTYPE_USER_NEWCVS,
	IMS_CALLBACKTYPE_RELAY_MSG,

	IMS_CALLBACKTYPE_END


};

// 回调函数
typedef void(CALLBACK* IMS_CALLBACK_USER_NEWCVS)(
		IMS_CONTEXT_TYPE& context,
		uint16	ret);

typedef void(CALLBACK* IMS_CALLBACK_SEND_MSG)(
		const IMS_CONTEXT_TYPE& context,
		const uint16& status_code
		);



#endif
