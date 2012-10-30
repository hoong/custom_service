/*
 * main.cpp
 *
 *  Created on: 2010-9-14
 *      Author: root
 */

//#include "command.h"
#include "cmd.h"
#include "ICSClient.h"
#include "ICSClientManager.h"
#include "packet/in_packet.h"
#include "packet/out_packet.h"
#include "CSClient.h"
#include "CSClientManager.h"
#include "CallbackQueue.h"
#include "CallBack.h"

#include <boost/program_options.hpp>
#include <iostream>
#include <signal.h>
#include <execinfo.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SVR_INIT(name, cond) { \
	LOG(fatal)<<name<<" initializing ................. "<<ENDL; \
	if (!cond) { LOG(fatal)<<name <<" init fail!!!"<<ENDL; return 0; } \
		LOG(fatal)<<name<<" init success!"<<ENDL; }


accept_context global_ac;

int main(int args, char* argv[])
{
	std::string conffile;
	try
	{
		boost::program_options::options_description desc("params");
		desc.add_options()
			("conf", boost::program_options::value(&conffile)->default_value("custom_service_client.conf"), "configure file") ;

		boost::program_options::variables_map vm;
		boost::program_options::store(boost::program_options::parse_command_line(args, argv, desc), vm);
		boost::program_options::notify(vm);
	}
	catch (std::exception& e)
	{
		fprintf(stderr, "ERROR: command line: %s\n", e.what());
		return 0;
	}

	// load configure file

	// init logger
	//initLog(conffile.c_str());
	base::log_init(0,NULL, 10*1024*1024,10);

	// Init threadpool
		SVR_INIT("CS_THR_POOL", cs_client::CS_THR_POOL::instance().initialize(8));

		//cs_client::CS_CB_QUEUE::instance();
		//cs_client::CS_NET_MGR::instance().initialize();
	std::string remote = "192.168.94.21:2012";
	//sendmsg_context global_sc;

	ICSClientManager* pman = ICSClientManager::CreateInstance();
	pman->Start();
	sleep(1);
	//ICSClient* pClient = pman->CreateICSClient("192.168.94.21:2012");
	ICSClient* pClient = pman->CreateICSClient(remote.c_str());

	if (pClient)
	{
		UMID user,agent;
		user.multptinfo.imsid=  1;
		user.multptinfo.multptindex =  9;
		user.uin = 100;
		LOG(trace)<<"USER'S UMID:{"<<user.uin<<","<<user.multptvalue<<"}"<<ENDL;

		agent.uin = 101;
		agent.multptinfo.imsid=  1;
		agent.multptinfo.multptindex =  99;
		
		uint32 nQueueid = 11;
		uint64 nUserUid = 100;
		//uint64 nUserMultpt = 1;
		uint32 nAddr = 1000;
		
		sleep(5);

	int res =	pClient->Request_UserJoinMsg(
			nQueueid,
			user.uin, 
			user.multptvalue, 
			nAddr, 
			UserJoinCallback, 
			(void*)111);
	if(res ==0)
	printf("send sucsuss!\n");

res = pClient->Request_UserJoinMsg(
			nQueueid,
			user.uin, 
			user.multptvalue, 
			nAddr, 
			UserJoinCallback, 
			(void*)111);

	if(res ==0)
	printf("send sucsuss!\n");


res =pClient->Request_Agent_Presence(
		nQueueid,
		agent.uin,
		agent.multptvalue,
		10,
		agent_presence_callback,	//锟截碉拷锟斤拷锟斤拷
		NULL);
	
	if(res ==0)
	printf("send sucsuss!\n");
	sleep(1);

res = pClient->Request_Agent_Absence(
		nQueueid,
		agent.uin,
		agent.multptvalue,
		agent_absence_callback ,	//锟截碉拷锟斤拷锟斤拷
		NULL);

	if(res ==0)
	printf("send sucsuss!\n");
	sleep(1);

res =pClient->Request_Agent_Presence(
		nQueueid,
		agent.uin,
		agent.multptvalue,
		10,
		agent_presence_callback,	//锟截碉拷锟斤拷锟斤拷
		NULL);
	
	if(res ==0)
	printf("send sucsuss!\n");
	sleep(1);


	global_ac.agent = agent;
	global_ac.client = pClient;

res = pClient->Request_Agent_Accept(
		nQueueid,
		agent.uin,
		agent.multptvalue,
		agent_accept_callback,	//锟截碉拷锟斤拷锟斤拷
		&global_ac);
	
	if(res ==0)
	printf("send sucsuss!\n");

	sleep(5);

	/*
res = pClient->Request_Agent_Offline(
		nQueueid,
		agent.uid,
		agent.multpt,
		agent_offline_callback ,	//锟截碉拷锟斤拷锟斤拷
		NULL);
*/


	if(res ==0)
	printf("send sucsuss!\n");

	sleep(1);







		std::cin >> nUserUid;
	}

	LOG(fatal) << "end! aaaaaa"<< ENDL;

	while(1);
	cs_client::CS_NET_MGR::instance().join();

	return 0;
}

