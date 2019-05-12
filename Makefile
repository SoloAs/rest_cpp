#locations
CIVETWEB = src/civetweb-git
LLAPI = src/ll-api

#target
PROG = rest_cpp
SRC = $(wildcard src/rest/*.cpp)

#libs
CIVETWEB_LIB = libcivetweb.a
LLAPI_LIB = libllapi.so

CFLAGS = -std=c++11 -Iinclude $(COPT) -fPIC -Wall -O2 -g
LIBS = -lpthread

include $(CIVETWEB)/resources/Makefile.in-os
ifeq ($(TARGET_OS),LINUX)
        LIBS += -ldl
endif

LIBS += -lllapi

all: $(PROG)

$(PROG): $(CIVETWEB_LIB) $(LLAPI_LIB) $(SRC)
	$(CXX) -Llib -Wl,-rpath=/home/alexander/work/pro-av-work/rest/lib -o bin/$@ $(CFLAGS) $(LDFLAGS) $(SRC) lib/$(CIVETWEB_LIB) $(LIBS)


$(CIVETWEB_LIB):
	$(MAKE) -C $(CIVETWEB) clean lib WITH_CPP=1
	cp $(CIVETWEB)/$(CIVETWEB_LIB) lib

$(LLAPI_LIB):
	$(MAKE) -C $(LLAPI)
	

clean:
	rm -f build/* bin/* lib/*
	
.PHONY: all clean

