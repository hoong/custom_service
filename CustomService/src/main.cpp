/*
 * main.cpp
 *
 *  Created on: 2010-8-17
 *      Author: root
 */

#include <boost/program_options.hpp>
#include <iostream>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <execinfo.h>
#include "ConfigureMgr.h"
#include "logger.h"
#include "CSNetMgr.h"
#include "CSThreadPool.h"
#include "CSQueueMgr.h"
#include "CSImsMgr.h"


#define SVR_INIT(name, cond) { \
	LOG(fatal)<<name<<" initializing ................. "<<ENDL; \
	if (!cond) { LOG(fatal)<<name <<" init fail!!!"<<ENDL; return 0; } \
		LOG(fatal)<<name<<" init success!"<<ENDL; }


// Obtain a backtrace and print it
void dummy(int signo)
{
	void* array[32];
	size_t size = backtrace(array, 32);
	char** strings = backtrace_symbols(array, size);

	LOG(fatal)<<"signal:"<<signo<<".\n"<<"Obtained "<<size<<" stack frames."<<ENDL;

	for (size_t i = 0; i < size; i++)
		LOG(fatal)<<"frame"<<i+1<<":"<<strings[i]<<ENDL;

	free(strings);
}

// signal handle
volatile sig_atomic_t fatal_error_in_progress = 0;
void signal_handler(int signo)
{
	LOG(fatal)<<"signal_handler: "<<signo<<"!"<<ENDL;

	if (fatal_error_in_progress)
		raise(signo);

	fatal_error_in_progress = 1;

	if (signo == SIGTERM)
	{

		kill(getpid(), SIGKILL);
	}
	else
	{
		if (signo == SIGSEGV || signo == SIGABRT)
		{
			dummy(signo);
		}
		signal(signo, SIG_DFL);
		raise(signo);
	}
}

// Make the current process a UNIX daemon.
/*
inline int daemonize()
{
	pid_t pid = ::fork();

	if (pid == -1)
		return -1;
	else if (pid != 0)
		::exit(0); // Parent exits.

	// 1st child continues.
	::setsid(); // Become session leader.
	::signal(SIGHUP, SIG_IGN);

	pid = ::fork();
	if (pid != 0)
		::exit(0); // First child terminates.

	// Second child continues.

	::umask(0); // clear our file mode creation mask.

	int fd = ::open("/dev/null", O_RDWR, 0);
	if (fd != -1) {
		::dup2(fd, 0);
		//::dup2(fd, 1);
		::dup2(fd, 2);

		if (fd > 2)
			::close(fd);
	}

	return 0;
}
*/


int main(int args, char* argv[])
{
	// handle signals

	struct sigaction act;
	act.sa_handler = signal_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	for (int i=1; i<=31; ++i)
	{
		sigaction(i, &act, (struct sigaction*)NULL);
	}
	signal(SIGPIPE, SIG_IGN);


	// parse command line

	//int loglevel;
	std::string conffile;
	try
	{
		boost::program_options::options_description desc("params");
		desc.add_options()
			("conf", boost::program_options::value(&conffile)->default_value("custom_service.conf"), "configure file") ;

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
	SVR_INIT("CONFIGURE_MGR", CONFIGURE_MGR::instance().init(conffile.c_str()));

	// 守护进程
	if (CONFIGURE_MGR::instance().m_log_daemonize)
	{
		base::util::daemonize();
	}


	// init logger
	//initLog(conffile.c_str());
	/*
	std::cout<<"log_init:"<<base::log_init(CONFIGURE_MGR::instance().m_log_level,CONFIGURE_MGR::instance().m_log_filename.c_str(),
			CONFIGURE_MGR::instance().m_log_rotation_size,CONFIGURE_MGR::instance().m_log_rotation_num)<<endl;
			*/

	std::cout<<"log_init:"<<base::log_init(CONFIGURE_MGR::instance().m_log_level,NULL,
			CONFIGURE_MGR::instance().m_log_rotation_size,CONFIGURE_MGR::instance().m_log_rotation_num)<<endl;


	LOG(trace)<<"test logger";

	// Init threadpool
	SVR_INIT("CS_THR_POOL", custom_service::CS_THR_POOL::instance().initialize(CONFIGURE_MGR::instance().m_threadpool_size));
	SVR_INIT("CS_IMS_THR_POOL", custom_service::CS_IMS_THR_POOL::instance().initialize(CONFIGURE_MGR::instance().m_threadpool_size));

	SVR_INIT("CS_IMS_MGR", CSIMSMGR::instance().init());
	boost::shared_ptr<CSImsHandler> imsHandler;
	if (!CSIMSMGR::instance().get_handler(1,imsHandler))
	{
		LOG(fatal)<<"Connect to ims fail!\n"<<ENDL;
		exit(-1);
	}


	// Init threadpool
	SVR_INIT("CSMGR", custom_service::CSMGR::instance().initialize(conffile.c_str()));

	// Start CS
	SVR_INIT("CS_NET_MGR", custom_service::CS_NET_MGR::instance().initialize());


	LOG(fatal)<<"\n\n\n -----custom_service started!--------\n\n\n"<<ENDL;


	custom_service::CS_NET_MGR::instance().join();

	return 0;
}
