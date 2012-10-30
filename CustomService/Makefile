CXX=g++
CC=gcc

CXX_FLAG= -O0 -g3 -Wall -fmessage-length=0 -MMD -MP -DHAVE_CXX_STDHEADERS -DBOOST_FILESYSTEM_VERSION=2 

#to be configured
SERVERHOME=/home/91u/91userver
BOOST_HOME=/usr/local/boost_1_48_0


COMMON_PROJECT=$(SERVERHOME)/common_project


INCLUDE=-I$(BOOST_HOME)/include \
		-I/usr/local/include \
		-I./include \
		-I/$(SERVERHOME)/new/base/include \
		-I./src \
		-I../CustomServiceMsg/include \
		-I$(SERVERHOME)/new/ims/commandlib/include \
		-I/usr/include/mysql \
		-I/usr/include/mysql++ \
		-I$(COMMON_PROJECT) \
		-I$(SERVERHOME) \
		-I$(SERVERHOME)/thirdparty/tbb/include

LIB=-L$(SERVERHOME)/lib \
	-L$(SERVERHOME)/new/base/lib \
	-L$(BOOST_HOME)/lib \
	-L/usr/local/mysql \
	-L/usr/local/lib \
	-L/usr/lib/mysql \
    -lnewbase -lcommand \
	-ltbbmalloc -ltbb \
    -levent  -levent_pthreads  -ltcmalloc\
    -lboost_program_options -lboost_filesystem -lboost_thread -lboost_iostreams -lboost_date_time -lboost_system -lboost_log\
	-llog4cpp -lcurses -lexpat  -lmysqlpp -lmysqlclient \
    -lz -lbz2 -ldl -llzma -lrt -lpthread -lm -lc -lresolv -lnsl 

SRC=$(shell echo src/*.cpp)
OBJ=$(foreach n,$(SRC),obj/$(basename $(notdir $(n))).o)

OBJS=$(OBJ)


obj/%.o:src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAG) -c $(INCLUDE) $< -o $@

#$(OBJS):%.o:$(subst obj,src,%.cpp)
#	@echo $@ $<
#	@mkdir -p $(dir $@)
#	$(CXX) $(CXX_FLAG) -c $(INCLUDE) $< -o $@


.PHONY:CustomService


CustomService:$(OBJS)
	$(CXX) $^ -o $@ $(LIB)
	@echo "Compiled successfully ..."
	@cp $@ $(SERVERHOME)/bin

clean:
	@rm -rf $(OBJS) CustomService 

