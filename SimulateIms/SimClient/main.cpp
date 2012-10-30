#include "net/reactor.h"
#include "net/handler.h"
#include "packet/header.h"
#include "ImsCmd.h"
#include "type.h"
#include "logger.h"

#include <vector>

#include "ims_msg/CSIMSMsg_Agent_Delcvs.h"
#include "ims_msg/CSIMSMsg_Invite_List.h"
#include "ims_msg/CSIMSMsg_Usercvs_List.h"
#include "ims_msg/CSIMSMsg_User_List.h"
#include "ims_msg/CSIMSMsg_User_Newcvs.h"

#include "boost/shared_ptr.hpp"
#include "boost/bind.hpp"
#include "boost/thread.hpp"


#include <iostream>


class sim_client_handler:public base::NetHandler<sim_client_handler,base::packet::Header>
{
public:
	int handle_packet(base::packet::Header& header,base::packet::InPacket& in_ptr)
	{
		LOG(fatal)<<"Received Response"<<ENDL;
		return 1;
	};


};



void reactor_loop(boost::shared_ptr<base::NetReactor>& reactor_ptr)
{
	reactor_ptr->run_loop();
};



int main()
{
	base::log_init(0,NULL,10*1024*1024,10);
	//base::NetReactor reactor(new base::NetStrategyHandlerCreatorT<sim_client_handler>());
	boost::shared_ptr<base::NetReactor> reactor_ptr(new base::NetReactor(new base::NetStrategyHandlerCreatorT<sim_client_handler>()));

	reactor_ptr->init();

	timeval tv={3600,0};
	boost::shared_ptr<base::NetHandlerBase> spHandler;
	reactor_ptr->connect("127.0.0.1:2014",&tv,spHandler);

	boost::thread reactor_thread(boost::bind(reactor_loop,reactor_ptr));
	reactor_thread.detach();

	boost::shared_ptr<sim_client_handler> handler = boost::dynamic_pointer_cast<sim_client_handler>(spHandler);

	base::packet::Header header(WY_CS2IMS_INVITE_LIST);
	CSIMSMsg_Request_Invite_List request;
	request.m_nqueue = 999;
	base::packet::OutPacket out;
	request.encode(out);
	out<<request.m_nqueue;
	handler->send_packet(header,out);

	return 0;

};
