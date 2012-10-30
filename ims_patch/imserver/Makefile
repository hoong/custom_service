CXX=g++
CC=gcc

CXX_FLAG= -O0 -g3 -Wall -fmessage-length=0 -MMD -MP -DHAVE_CXX_STDHEADERS -DBOOST_FILESYSTEM_VERSION=2 

#to be configured
SERVERHOME=/home/91u/91userver
BOOST_HOME=/usr/local/boost_1_48_0
IMS_HOME=$(SERVERHOME)/new/ims
CUSTOM_SERVICE_HOME=$(SERVERHOME)/new/custom_service


COMMON_PROJECT=$(SERVERHOME)/common_project


INCLUDE=-I$(BOOST_HOME)/include -I/usr/local/include -I./include -I/$(SERVERHOME)/new/base/include -I./src \
		-I$(SERVERHOME)/status_service64/StatusServiceClient/util -I$(SERVERHOME)/status_service64/StatusServiceClient/api \
		-I$(IMS_HOME)/ -I$(IMS_HOME)/commandlib/include -I$(IMS_HOME)/mssclient/include  -I$(IMS_HOME)/mssmsglib/include \
		-I$(IMS_HOME)/netbaselib/include -I$(IMS_HOME)/netbaselib/include -I$(IMS_HOME)/mx2imclient/include \
		-I$(SERVERHOME)/new/third-party -I$(CUSTOM_SERVICE_HOME)/CustomServiceClient/include \
		-I$(CUSTOM_SERVICE_HOME)/CustomServiceMsg/include \
		-I$(IMS_HOME)/../group -I$(IMS_HOME)/../ -I$(SERVERHOME)/sms -I$(SERVERHOME)/group_message_service/GroupMessageServiceClient/util\
		-I/usr/include/mysql -I/usr/include/mysql++ -I$(COMMON_PROJECT) -I$(SERVERHOME) -I$(SERVERHOME)/thirdparty/tbb/include 

LIB=-L$(SERVERHOME)/lib \
	-L$(SERVERHOME)/new/base/lib \
	-L$(BOOST_HOME)/lib \
	-L/usr/local/mysql \
	-L/usr/local/lib \
	-L/usr/lib/mysql \
    -lnewbase -lcommand \
	-ltbbmalloc -ltbb -lmcmclient -lmntclient -lmssclient -lStatusServiceClient -lnetbaselib \
	-lcommandlib -lcommonutil -lNetDragonLogger  -lbase -lCSClient\
    -levent  -levent_pthreads  -ltcmalloc -lSystemInfo -lgroupmessageserviceclient \
    -lboost_program_options -lboost_filesystem -lboost_thread -lboost_iostreams -lboost_date_time -lboost_system -lboost_log \
	-llog4cpp -lcurses -lexpat  -lmysqlpp -lmysqlclient -luap -lmysqlppconnector -lgroup_client \
	-lACE -lz -lbz2 -ldl -llzma -lrt -lpthread -lm -lc -lresolv -lnsl -lcurl -ljson -lnewbase

OUTDATE_SRC = src/UserProcessSystemMsg.cpp src/server/SMSManager.cpp src/statemachine/SM_GroupMsgRelay.cpp


SRC=$(filter-out $(OUTDATE_SRC), $(shell echo src/*.cpp))
OBJ=$(foreach n,$(SRC),obj/$(basename $(notdir $(n))).o)

DB_SRC=$(filter-out $(OUTDATE_SRC),$(shell echo src/db/*.cpp))
DB_OBJ=$(foreach n,$(DB_SRC),obj/db/$(basename $(notdir $(n))).o)

IMS2S_SRC=$(filter-out $(OUTDATE_SRC),$(shell echo src/ims2s/*.cpp))
IMS2S_OBJ=$(foreach n,$(IMS2S_SRC),obj/ims2s/$(basename $(notdir $(n))).o)

S2MXS_SRC=$(filter-out $(OUTDATE_SRC),$(shell echo src/s2mxs/*.cpp))
S2MXS_OBJ=$(foreach n,$(S2MXS_SRC),obj/s2mxs/$(basename $(notdir $(n))).o)

S2UNAS_SRC=$(filter-out $(OUTDATE_SRC),$(shell echo src/s2unas/*.cpp))
S2UNAS_OBJ=$(foreach n,$(S2UNAS_SRC),obj/s2unas/$(basename $(notdir $(n))).o)

SERVER_SRC=$(filter-out $(OUTDATE_SRC),$(shell echo src/server/*.cpp))
SERVER_OBJ=$(foreach n,$(SERVER_SRC),obj/server/$(basename $(notdir $(n))).o)

STATEMACHINE_SRC=$(filter-out $(OUTDATE_SRC),$(shell echo src/statemachine/*.cpp))
STATEMACHINE_OBJ=$(foreach n,$(STATEMACHINE_SRC),obj/statemachine/$(basename $(notdir $(n))).o)

UAP_SRC=$(filter-out $(OUTDATE_SC),$(shell echo src/uap/*.cpp))
UAP_OBJ=$(foreach n,$(UAP_SRC),obj/uap/$(basename $(notdir $(n))).o)








OBJS=$(OBJ) $(DB_OBJ) $(IMS2S_OBJ) $(S2MXS_OBJ) $(S2UNAS_OBJ) $(SERVER_OBJ) $(STATEMACHINE_OBJ) $(UAP_OBJ)




obj/%.o:src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAG) -c $(INCLUDE) $< -o $@

obj/db/%.o:src/db/%.cpp
	@echo $(SRC)
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAG) -c $(INCLUDE) $< -o $@

obj/ims2s/%.o:src/ims2s/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAG) -c $(INCLUDE) $< -o $@

obj/s2mxs/%.o:src/s2mxs/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAG) -c $(INCLUDE) $< -o $@

obj/s2unas/%.o:src/s2unas/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAG) -c $(INCLUDE) $< -o $@

obj/server/%.o:src/server/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAG) -c $(INCLUDE) $< -o $@

obj/statemachine/%.o:src/statemachine/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAG) -c $(INCLUDE) $< -o $@

obj/uap/%.o:src/uap/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAG) -c $(INCLUDE) $< -o $@




.PHONY:imserver


imserver:$(OBJS)
	@echo $(SRC)
	$(CXX) $^ -o $@ $(LIB)
	@echo "Compiled successfully ..."
	@cp $@ $(SERVERHOME)/bin

clean:
	@rm -rf $(OBJS) imserver 

