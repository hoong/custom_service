CXX=g++
CC=gcc

CXX_FLAG= -O0 -g3 -Wall -fmessage-length=0 -MMD -MP -DHAVE_CXX_STDHEADERS -DBOOST_FILESYSTEM_VERSION=2 

#to be configured
SERVERHOME = /root/main
COMMON_PROJECT=$(SERVERHOME)/custom_service/client


#build project depend on home below
BOOST_HOME = /usr/local/boost_1_48_0
LIBEVENT_HOME = /usr/local/libevent_2.0.16
MYSQL_HOME = /usr/local/mysql_15.0.0
MYSQLPP_HOME = /usr/local/mysqlpp_3.1.0

#lib in upfloder 
NETBASE_HOME = $(SERVERHOME)/base



INCLUDE= -I/usr/include \
			-I/usr/local/include \
			-I$(BOOST_HOME)/boost \
			-I$(LIBEVENT_HOME)/include \
			-I$(MYSQL_HOME)/mysql \
			-I$(MYSQLPP_HOME) \
			-I$(NETBASE_HOME)/include \
			-I$(COMMON_PROJECT)/include \
			-I$(SERVERHOME) \
			-I$(COMMON_PROJECT) \
			-I./src \
			-I../CustomServiceMsg/include \
			-I../CustomServiceCmd/include
		   

LIB = -L/usr/lib \
		-L/usr/local/lib \
		-L$(BOOST_HOME)/lib \
		-L$(LIBEVENT_HOME)/lib \
		-L$(MYSQL_HOME)/lib \
		-L$(MYSQLPP_HOME)/lib \
		-L$(NETBASE_HOME)/lib \
		-lboost_program_options -lboost_filesystem -lboost_thread \
	 	-lboost_iostreams -lboost_date_time -lboost_system -lboost_log \
    	-levent  -levent_pthreads \
		-lmysqlpp -lmysqlclient \
		-lnewbase \
    	-lz -lbz2 -ldl -lrt -lpthread -lm -lc -lresolv -lnsl 

SRC=$(shell echo src/*.cpp) 

OBJ=$(foreach n,$(SRC),obj/$(basename $(notdir $(n))).o)

OBJS=$(OBJ)


obj/%.o:src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAG) -c $(INCLUDE) $< -o $@


.PHONY:CustomServiceClient


CustomServiceClient:$(OBJS)
	$(CXX) $^ -o $@ $(LIB)
	@echo "Compiled successfully ..."
	@cp $@ $(SERVERHOME)/bin

clean:
	@rm -rf $(OBJS) CustomServiceClient 

