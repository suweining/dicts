CPP			= /opt/gcc-4.7.2/bin/g++
BUILD	   ?= debug
ifeq (debug, $(shell tr '[:upper:]' '[:lower:]' <<< $(BUILD)))
	override BUILD = debug
	CPPFLAGS = $(INCLUDE) -fPIC -g -std=gnu++11
else
	override BUILD = release
	CPPFLAGS = $(INCLUDE) -fPIC -O2
endif

HOME		=	/home/suweining/workplace/dicts
THIRD_PARTY	=	$(HOME)/third_party
LIB_GTEST	=	$(THIRD_PARTY)/gtest-1.7.0
LIB_AHOCORASICK	= 	$(THIRD_PARTY)/ahocorasick
LIB_RE		=	$(THIRD_PARTY)/re2
LIB_THRIFT	=	$(THIRD_PARTY)/thrift-0.9.1/


INCLUDE		=	\
			-I$(HOME)/include \
			-I$(LIB_AHOCORASICK)/include \
			-I$(LIB_GTEST)/include \
			-I$(LIB_RE)/include \
			-I$(LIB_THRIFT)/include \

LIBS		=	\
			-L$(HOME)/lib \
			-L$(LIB_RE)/lib \
			$(LIB_AHOCORASICK)/lib/libahocorasick.a \
			$(LIB_GTEST)/lib/libgtest.a \
			$(LIB_THRIFT)/lib/libthrift.a \
			$(LIB_THRIFT)/lib/libthriftnb.a \

LDFLAGS		= 	$(LIBS) -lboost_thread-mt -lboost_system-mt -lre2 -Wl,-rpath=../lib -Wl,-rpath=./lib


OBJS1		= 	ac_automachine_dict.o  ac_matcher.o util.o log.o mmap_reader.o  pattern_dict.o main.o
TARGET1		= 	server

TEST_OBJS1	=	unit/ac_matcher_unit.o 
TEST_EXE1	=	unit/ac_matcher_unit

TEST_OBJS2	=	mmap_reader.o unit/mmap_reader_unit.o
TEST_EXE2	=	unit/mmap_reader_unit


ALL_TEST	=	$(TEST_EXE1)  $(TEST_EXE2)
ALL_TARGETS	= 	$(TARGET1) 

all: $(ALL_TARGETS)

test: $(ALL_TEST)

.cpp.o:
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(TARGET1): $(OBJS1)
	$(CPP) -o $@ $^ $(LDFLAGS)

$(TEST_EXE1): $(TEST_OBJS1)
	$(CPP) -o $@ $^ $(LDFLAGS)
	$(TEST_EXE1)

$(TEST_EXE2): $(TEST_OBJS2)
	$(CPP) -o $@ $^ $(LDFLAGS)
	$(TEST_EXE2)

.PHONY: clean
clean:
	rm -f $(HOME)/src/*.o $(HOME)/$(ALL_TARGETS) $(HOME)/$(ALL_TEST)
