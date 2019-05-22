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

$(PROG): $(CIVETWEB_LIB) $(LLAPI_LIB) $(SRC) test nmos gui
	$(CXX) -Llib -Wl,-rpath=/home/alexander/work/pro-av-work/rest/lib -o bin/$@ $(CFLAGS) $(LDFLAGS) $(SRC) lib/$(CIVETWEB_LIB) $(LIBS)


$(CIVETWEB_LIB):
	$(MAKE) -C $(CIVETWEB) clean lib WITH_CPP=1
	cp $(CIVETWEB)/$(CIVETWEB_LIB) lib

$(LLAPI_LIB):
	$(MAKE) -C $(LLAPI)
	

rest:
	rm -f bin/rest_cpp
	$(CXX) -Llib -Wl,-rpath=/home/alexander/work/pro-av-work/rest/lib -o bin/$@ $(CFLAGS) $(LDFLAGS) $(SRC) lib/$(CIVETWEB_LIB) $(LIBS) && mv bin/rest bin/rest_cpp

nmos: 
	cd apps/nmos && npm install --arch=armv7 --only=prod

gui:
	cd apps/gui && npm install --arch=armv7 --only=prod
	
test:
	cd test/rest && npm install --arch=armv7 --only=prod

clean:
	rm -f build/* bin/* lib/* test/rest/package-lock.json apps/nmos/package-lock.json apps/gui/package-lock.json
	rm -rf test/rest/node_modules
	rm -rf apps/nmos/node_modules
	rm -rf apps/gui/node_modules

	
.PHONY: all clean test

