#include "net/reactor.h"
#include "net/handler.h"
#include "packet/header.h"
#include "ImsCmd.h"
#include "type.h"
#include "logger.h"
#include <vector>
#include "thread_pool.h"

#include "ims_msg/CSIMSMsg_Agent_Delcvs.h"
#include "ims_msg/CSIMSMsg_Invite_List.h"
#include "ims_msg/CSIMSMsg_Usercvs_List.h"
#include "ims_msg/CSIMSMsg_User_List.h"
#include "ims_msg/CSIMSMsg_User_Newcvs.h"
#include "ims_msg/CSIMSMsg_Relay_Msg.h"


#include <iostream>
#include <boost/thread/detail/singleton.hpp>


typedef  boost::detail::thread::singleton<base::ThreadPool> TPMGR;


struct test_bind
{
	test_bind()
	{
		LOG(fatal)<<"test_bind Conduct:"
			<<this<<ENDL;
	}

	~test_bind()
	{
		LOG(fatal)<<"test_bind Deduct:"
			<<this<<ENDL;
	}

	void addr()
	{
		LOG(fatal)<<"test_bind addr:"<<this<<ENDL;
	}

	test_bind(const test_bind& tb)
	{
		LOG(fatal)<<"test_bind copy from:"<<&tb
			<<" to:"<<this<<ENDL;
	};

};

void call_test_bind(test_bind& tb)
{
	tb.addr();
};



class simims_handler:public base::NetHandler<simims_handler,base::packet::Header>
{
public:
	simims_handler()
	{
	};

	virtual int handle_packet(base::packet::Header& header,base::packet::InPacket& in)
	{

		boost::shared_ptr<base::packet::InPacket> in_ptr(new base::packet::InPacket());
		in_ptr->swap(in);
		LOG(fatal)<<"inpacket swap successful"<<ENDL;

		/*
		base::packet::InPacket &in = *in_ptr;

		{
			base::packet::InPacket in2(in);
			base::packet::InPacket in3(in2);
			LOG(fatal)<<"in2.size="<<in2.get_size()<<ENDL;
			base::packet::InPacket in4(in3);
			base::packet::InPacket in5(in4);
			base::packet::InPacket in6(in5);
			base::packet::InPacket in7(in6);
			base::packet::InPacket in8(in7);
			base::packet::InPacket in9(in8);
		}
		LOG(fatal)<<"body_size="<<header.body_size()<<",real_body_size="<<in.get_size()<<ENDL;

		LOG(fatal)<<"addr of inpacket:"<<&in<<ENDL;
		*/

		//TPMGR::instance().schedule(&parse_imsmsg,header,&i);
		TPMGR::instance().schedule(&simims_handler::parse_imsmsg,this->shared_from_this(),header,in_ptr);
		//parse_imsmsg(header,in);

		return 1;

	}


	void parse_imsmsg(base::packet::Header& header,boost::shared_ptr<base::packet::InPacket>& in_ptr)
	{
		uint16 ret = 200;
		header.status_code(ret);
		base::packet::InPacket &in = *in_ptr;
		uint16 cmd = header.cmd();
		LOG(fatal)<<"Receive Command:";
		switch(cmd)
		{

			case WY_CS2IMS_INVITE_LIST:
				{
					CSIMSMsg_Request_Invite_List request;
					request.decode(in);
					LOG(fatal)<<"WY_CS2IMS_INVITTE_LIST"<<ENDL;
					break;
				}
			case WY_CS2IMS_USER_LIST:
				{
					CSIMSMsg_Request_User_List request;
					request.decode(in);
					LOG(fatal)<<"WY_CS2IMS_USER_LIST"<<ENDL;
					break;
				}
			case WY_CS2IMS_USERCVS_LIST:
				{
					CSIMSMsg_Request_Usercvs_List request;
					request.decode(in);
					LOG(fatal)<<"WY_CS2IMS_USERCVS_LIST"<<ENDL;
					break;
				}
			case WY_CS2IMS_AGENT_DELCVS:
				{
					CSIMSMsg_Request_Agent_Delcvs request;
					request.decode(in);
					LOG(fatal)<<"WY_CS2IMS_AGENT_DELCVS"<<ENDL;
					break;
				}
			case WY_CS2IMS_USER_NEWCVS:
				{
					CSIMSMsg_Request_User_Newcvs request;
					request.decode(in);
					LOG(fatal)<<"WY_CS2IMS_USER_NEWCVS"<<ENDL;
					break;
				}
			case WY_CS2IMS_MSG:
				{
					CSIMSMsg_Request_Relay_Msg request;
					request.decode(in);
					LOG(fatal)<<"WY_CS2IMS_MSG:"<<request.m_text<<ENDL;
					break;
				}
			default:
				{
					LOG(fatal)<<"UNKNOWN COMMAND"<<ENDL;
					break;
				}
				
		}

		send_packet(header,ret);
	
	};
	



};







int main()
{
	
	LOG(fatal)<<"begin"<<ENDL;
	TPMGR::instance().start(1);
	test_bind tb;
	boost::bind(&call_test_bind,tb)();
	base::log_init(0,NULL,10*1024*1024,10);
	base::NetReactor reactor(new base::NetStrategyHandlerCreatorT<simims_handler>());
	reactor.init();
	reactor.listen(2014);
	reactor.run_loop();
	return 0;
};



