.PHONY:clean all cppcheck

CXX=g++
INCLUDE_DIR=./include
CXX_FLAGS=-std=c++11
SUBDIRS=$(shell ls -l | grep ^d | awk '{if($$9 != "debug") if($$9 != "include") print $$9}')
ROOT_DIR=$(shell pwd)
BIN=hierarchy
OBJS_DIR=debug/obj
BIN_DIR=debug/bin
CUR_SOURCE=${wildcard *.cpp}
CUR_OBJS=${patsubst %.cpp, %.o, $(CUR_SOURCE)}

CPPCHECK = cppcheck
CHECKFLAGS = -q --enable=all --error-exitcode=1 --suppress=missingIncludeSystem:*
SOURCES = $(wildcard *.cpp) $(wildcard */*.cpp)

export CXX BIN OBJS_DIR BIN_DIR ROOT_DIR

all:$(SUBDIRS) $(CUR_OBJS) DEBUG
$(SUBDIRS):ECHO
	make -C $@

DEBUG:ECHO
	make -C debug

ECHO:
	@echo $(SUBDIRS)
	@echo $(SOURCES)


$(CUR_OBJS):%.o:%.cpp
	$(CXX) -c $^ -o $(ROOT_DIR)/$(OBJS_DIR)/$@ $(CXX_FLAGS) -I $(INCLUDE_DIR)

clean:
	rm -rf $(OBJS_DIR)/*.o
	rm -rf $(BIN_DIR)/*
	rm -rf ./*.out
	rm -rf ./tags

cppcheck:
	$(CPPCHECK) $(CHECKFLAGS) $(SOURCES)


