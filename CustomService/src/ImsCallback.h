#ifndef IMS_CALLBACK_H
#define IMS_CALLBACK_H


#include "ImsCallbackDef.h"
#include "ICSDbMgr.h"



struct user_newcvs_context:public i_ims_context
{

};





struct relaymsg_context:public i_ims_context
{
	MSG_INFO mi;
	boost::shared_ptr<custom_service::IDbMgr> dbmgr;
};

void relaymsg_callback(const boost::shared_ptr<i_ims_context>& context,const uint16& status_code);








#endif
