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
LIB_AHOCORASICK	= 	$(THIRD_PARTY)/ahocorasick
LIB_RE		=	$(THIRD_PARTY)/re2
LIB_THRIFT	=	$(THIRD_PARTY)/thrift-0.9.1/
LIB_RAPIDJSON	=	$(THIRD_PARTY)/rapidjson/


INCLUDE		=	\
			-I$(HOME)/include \
			-I$(LIB_AHOCORASICK)/include \
			-I$(LIB_GTEST)/include \
			-I$(LIB_RE)/include \
			-I$(LIB_THRIFT)/include \
			-I$(LIB_RAPIDJSON)/include \

LIBS		=	\
			-L$(HOME)/lib \
			-L$(LIB_RE)/lib \
			$(LIB_AHOCORASICK)/lib/libahocorasick.a \
			$(LIB_GTEST)/lib/libgtest.a \
			$(LIB_THRIFT)/lib/libthrift.a \
			$(LIB_THRIFT)/lib/libthriftnb.a \
			$(LIB_RAPIDJSON)/lib4c/librapidjson.a \


LDFLAGS		= 	$(LIBS) -lre2 -Wl,-rpath=../lib -Wl,-rpath=./lib


OBJS1		= 	ini.o  util.o log.o mmap_reader.o  pattern_dict.o spider_filter_pattern_key.o spider_filter_pattern_value.o spider_filter_kv_key.o spider_filter_kv_value.o  kv_dict.o dict_factory.o key_factory.o value_factory.o match_engine_unit.o match_engine_level.o match_engine_singleton.o
TARGET1		= 	../bin/libdicts.a

OBJS2		= 	function_test.o
TARGET2		= 	../bin/function_test

TEST_OBJS1	=	unit/ac_matcher_unit.o
TEST_EXE1	=	unit/ac_matcher_unit

TEST_OBJS2	=	mmap_reader.o unit/mmap_reader_unit.o
TEST_EXE2	=	unit/mmap_reader_unit

TEST_OBJS3	= 	unit/pattern_dict_struct_unit.o pattern_dict.o pattern_dict_struct_key.o pattern_dict_struct_value.o util.o log.o
TEST_EXE3	=	unit/pattern_dict_struct_unit

TEST_OBJS4	= 	unit/dict_handler_unit.o
TEST_EXE4	=	unit/dict_handler_unit



#ALL_TEST	=	$(TEST_EXE1)  $(TEST_EXE2) $(TEST_EXE4)
ALL_TARGETS	= 	$(TARGET1) $(TARGET2)

all: $(ALL_TARGETS)

#test: $(ALL_TEST)

.cpp.o:
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(TARGET1): $(OBJS1)
	ar cr $(TARGET1) $(OBJS1)

$(TARGET2): $(OBJS2)
	$(CPP) -o $@ $^ ../bin/libdicts.a $(LDFLAGS) 

$(TEST_EXE1): $(TEST_OBJS1)
	$(CPP) -o $@ $^ $(LDFLAGS)
	$(TEST_EXE1)

$(TEST_EXE2): $(TEST_OBJS2)
	$(CPP) -o $@ $^ $(LDFLAGS)
	$(TEST_EXE2)

$(TEST_EXE3):$(TEST_OBJS3)
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

$(TEST_EXE4):$(TEST_OBJS4)
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)


.PHONY: clean
clean:
	rm -f $(HOME)/src/*.o $(HOME)/$(ALL_TARGETS)  $(TARGET1) $(TARGET2) #$(HOME)/$(ALL_TEST)
