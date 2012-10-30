#ifndef CUSTOM_SERVICE_NET_HANDLER_H_
#define CUSTOM_SERVICE_NET_HANDLER_H_
#include "net/handler.h"
#include "packet/header.h"
#include "type.h"

namespace custom_service {

class CSNetHandler:public base::NetHandler<CSNetHandler,base::packet::Header> 
{

public:
	virtual int open();
	virtual void handle_close();
	virtual int handle_packet(base::packet::Header& header, base::packet::InPacket& in);

private:
	void processTask(base::packet::Header& header,boost::shared_ptr<base::packet::InPacket>& in);

	template <typename REQUEST,typename BSN_MGR>
	void processBSNRequest(REQUEST& req,BSN_MGR& bsnmgr,base::packet::Header& header,base::packet::InPacket& in);

	int sendErrCode(uint16 cmd,uint32 seq,uint16 err);

};

}

#endif
