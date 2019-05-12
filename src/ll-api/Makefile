# Makefile for LL API shared library

TOP = ../..

CFLAGS = -I$(TOP)/include  -std=c++11 -fPIC -Wall -O2 -g # C flags
LDFLAGS = -shared  # linking flags
TARGET_LIB = libllapi.so # target lib

SRCS = ll_api_class.cpp # source files

all: ${TARGET_LIB}

$(TARGET_LIB): $(OBJS)
	$(CXX) $(LDFLAGS) -o $(TARGET_LIB) $(CFLAGS) $(SRCS)
	mv $(TARGET_LIB) $(TOP)/lib

.PHONY: all 
