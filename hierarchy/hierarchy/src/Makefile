INCLUDE_DIR=-I../include
CXX_FLAGS=-std=c++11

SUBDIRS=$(shell ls -l | grep ^d | awk '{print $$9}')

CUR_SOURCE=${wildcard *.cpp}
CUR_OBJS=${patsubst %.cpp, %.o, $(CUR_SOURCE)}
all:$(SUBDIRS) $(CUR_OBJS)
$(SUBDIRS):ECHO
	make -C $@
$(CUR_OBJS):%.o:%.cpp
	$(CXX) $(INCLUDE_DIR) $(CXX_FLAGS) -c $^ -o $(ROOT_DIR)/$(OBJS_DIR)/$@
ECHO:
	@echo $(SUBDIRS)
