CC = gcc
CXX = g++
RM = rm -f

# m64 = 64 bit, m32 = 32 bit
ARCH = -m64 

# 0 = Release, anything else = Debug
DEBUG = 1

# 0 = Dynamic linking(standard), anything else = Static linking
STATIC = 0

# Linking settings
ifeq ($(STATIC), 0)
LINKING = $(shell sdl2-config --libs)
else
LINKING = $(shell sdl2-config --static-libs)
endif

# Release flags
ifeq ($(DEBUG), 0) 
CPP_EXTRA = -fomit-frame-pointer -flto -O2 -DRELEASE
LD_EXTRA = -flto -s
else
CPP_EXTRA = -pg -g
LD_EXTRA = -pg
endif

WARNINGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Wunreachable-code -Wmain -pedantic -Wfatal-errors -Wextra -Wall

CSTD = -std=c17

CPPFLAGS = $(shell root-config --cflags) $(shell sdl2-config --cflags) $(WARNINGS) $(CSTD) $(ARCH) $(CPP_EXTRA)
LDFLAGS = $(shell root-config --ldflags) $(LINKING) $(LD_EXTRA)

SRCS = $(wildcard *.c)
OBJS = $(SRCS : .c = .o)

all: FastSpace

FastSpace: $(OBJS)
	$(CC) $(LDFLAGS) -o FastSpace $(OBJS) $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CC) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) *~ .depend

include .depend