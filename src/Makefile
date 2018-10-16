CPP			= /opt/gcc-4.7.2/bin/g++
BUILD	   ?= debug
ifeq (debug, $(shell tr '[:upper:]' '[:lower:]' <<< $(BUILD)))
	override BUILD = debug
	CPPFLAGS = $(INCLUDE) -fPIC -g
else
	override BUILD = release
	CPPFLAGS = $(INCLUDE) -fPIC -O2
endif

HOME		=	/home/suweining/workplace/dicts
THIRD_PARTY	=	$(HOME)/third_party
LIB_GTEST	=	$(THIRD_PARTY)/gtest-1.7.0
LIB_AHOCORASICK	= 	$(THIRD_PARTY)/ahocorasick


INCLUDE		=	\
			-I$(HOME)/include \
			-I$(LIB_AHOCORASICK)/include \
			-I$(LIB_GTEST)/include \

LIBS		=	\
			-L$(HOME)/lib \
			$(LIB_AHOCORASICK)/lib/libahocorasick.a \
			$(LIB_GTEST)/lib/libgtest.a \

LDFLAGS		= 	$(LIBS) -lboost_thread-mt -lboost_system-mt -Wl,-rpath=../lib -Wl,-rpath=./lib


OBJS1		= 	ac_automachine_dict.o  ac_matcher.o util.o log.o main.o
TARGET1		= 	server

TEST_OBJS1	=	unit/ac_matcher_unit.o
TEST_EXE1	=	unit/ac_matcher_unit

ALL_TEST	=	$(TEST_EXE1)
ALL_TARGETS	= 	$(TARGET1) 

test: $(ALL_TEST)

all: $(ALL_TARGETS)

.cpp.o:
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(TARGET1): $(OBJS1)
	$(CPP) -o $@ $^ $(LDFLAGS)

$(TEST_EXE1): $(TEST_OBJS1)
	$(CPP) -o $@ $^ $(LDFLAGS)
	$(TEST_EXE1)


.PHONY: clean
clean:
	rm -f src/*.o $(ALL_TARGETS) $(ALL_TEST)
