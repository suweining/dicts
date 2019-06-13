CPP			= /opt/gcc-4.7.2/bin/g++
BUILD	   ?= debug
ifeq (debug, $(shell tr '[:upper:]' '[:lower:]' <<< $(BUILD)))
	override BUILD = debug
	CPPFLAGS = $(INCLUDE) -fPIC -g -std=gnu++11
else
	override BUILD = release
	CPPFLAGS = $(INCLUDE) -fPIC -O2
endif

HOME		=	/home/suweining/workplace/tmp/dicts
THIRD_PARTY	=	$(HOME)/third_party
LIB_GTEST	=	$(THIRD_PARTY)/gtest-1.7.0
LIB_TARGET	=	$(HOME)/libdicts/lib/libdicts.a
LIB_AHOCORASICK	= 	$(THIRD_PARTY)/ahocorasick
LIB_RE		=	$(THIRD_PARTY)/re2
LIB_THRIFT	=	$(THIRD_PARTY)/thrift-0.9.1/
LIB_RAPIDJSON	=	$(THIRD_PARTY)/rapidjson/
LIB_REDISCLIENT =	$(THIRD_PARTY)/redis_client/
LIB_HIREDIS	=	$(THIRD_PARTY)/hiredis/

INCLUDE		=	\
			-I$(HOME)/include \
			-I$(LIB_GTEST)/include \
			-I$(LIB_RE)/include \
			-I$(LIB_THRIFT)/include \
			-I$(LIB_RAPIDJSON)/include \
			-I$(LIB_HIREDIS)/include \
			-I$(LIB_REDISCLIENT)/include \

LIBS		=	\
			-L$(HOME)/lib \
			-L$(LIB_RE)/lib \
			-L$(LIB_REDISCLIENT)/lib \
			-L$(LIB_HIREDIS)/lib \
			$(LIB_TARGET) \
			$(LIB_AHOCORASICK)/lib/libahocorasick.a \
			$(LIB_GTEST)/lib/libgtest.a \
			$(LIB_THRIFT)/lib/libthrift.a \
			$(LIB_THRIFT)/lib/libthriftnb.a \
			$(LIB_RAPIDJSON)/lib4c/librapidjson.a \

LDFLAGS		= 	-lre2 -lhiredis -lredis_client -lpthread -lcrypto -Wl,-rpath=../lib -Wl,-rpath=./lib -Wl,-rpath=../lib $(LIBS)

TARGET		= 	./bin/unit_test
OBJECT		=	./src/run_unit.o



all: $(TARGET)

.cpp.o:
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(TARGET): $(OBJECT)
	$(CPP) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(HOME)/unit/src/*.o $(TARGET)
