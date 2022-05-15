CC = gcc
CXX = g++
RM = rm -f

# m64 = 64 bit, m32 = 32 bit
ARCH = -m64 

# 1 = Debug, Anything else = Release
DEBUG = 1

# 0 = Dynamic linking(standard), anything else = Static linking
STATIC = 0

# Linking settings
ifeq ($(STATIC), 0)
LINKING = $(shell sdl2-config --libs)
else
LINKING = $(shell sdl2-config --static-libs)

# Release flags
ifeq ($(DEBUG), 0) 
CPP_EXTRA = -fomit-frame-pointer -flto -O2 -DRELEASE
LD_EXTRA = -flto -s
else
CPP_EXTRA = -pg -g
LD_EXTRA = -pg
endif

CPPFLAGS = -g $(shell root-config --cflags) $(shell sdl2-config --cflags) -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Wunreachable-code -Wmain -pedantic -Wfatal-errors -Wextra -Wall -std=c17 
CPPFLAGS += $(ARCH) $(CPP_EXTRA)
LDFLAGS = $(shell root-config --ldflags)
LDLIBS += $(LINKING) $(LD_EXTRA)

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