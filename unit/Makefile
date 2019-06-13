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


INCLUDE		=	\
			-I$(HOME)/include \
			-I$(LIB_GTEST)/include \

LIBS		=	\
			-L$(HOME)/lib \
			$(LIB_GTEST)/lib/libgtest.a \


LDFLAGS		= 	$(LIBS) -lpthread -lcrypto -Wl,-rpath=../lib -Wl,-rpath=./lib -Wl,-rpath=../lib



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
