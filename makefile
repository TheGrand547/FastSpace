CC = gcc
CXX = g++
RM = rm -f

# m64 = 64 bit, m32 = 32 bit
ARCH = -m64 

DEBUG = true

# Release flags
ifeq (DEBUG, true) 
CPP_EXTRA = -fomit-frame-pointer -flto -O2 -DRELEASE
LD_EXTRA = -flto -s
else
CPP_EXTRA = -pg -g
LD_EXTRA = -pg
endif

CPPFLAGS = -g $(shell root-config --cflags) $(shell sdl2-config --cflags) -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Wunreachable-code -Wmain -pedantic -Wfatal-errors -Wextra -Wall -std=c17 
CPPFLAGS += ARCH 
CPPFLAGS :=
LDFLAGS = $(shell root-config --ldflags)
LDLIBS = $(shell root-config --libs) $(shell sdl2-config --libs)

SRCS = $(wildcard *.c)
OBJS = $(SRCS : .c = .o)

all: FastSpace

FastSpace: $(OBJS)
    $(CXX) $(LDFLAGS) -o FastSpace $(OBJS) $(LDLIBS)

depend: .depend

.depend: $(SRCS)
    $(RM) ./.depend
    $(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
    $(RM) $(OBJS)

distclean: clean
    $(RM) *~ .depend

include .depend