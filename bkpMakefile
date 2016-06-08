ARCH	=	`uname -m`

OUTDIR  =    	.
OUTNAME =    	offline
OUT     =    	$(OUTDIR)/$(OUTNAME)

CC	=	g++

#FLAGS	=	-soname -s
#FLAGS	=       -Wall,-soname -s
#FLAGS	=	-Wall,-soname -nostartfiles -s
FLAGS	=	-Wall -fPIC

ROOTCFLAGS  = $(shell root-config --cflags)

ROOTLIBS    = $(shell root-config --libs)

LIBS	=	-L.. -lCommandLineInterface

INCLUDEDIR =	-I$(ROOTSYS)/include

#SOURCEFILES = $(shell find ./src -type f -iname '*.cpp')
#SOURCEFILES= main.cpp
SOURCEFILES = $(shell find ./ -type f -iname '*.cpp')
OBJS= $(subst .cpp,.o,$(SOURCEFILES))

#INCLUDES =	./include/*
INCLUDES =	./*

#########################################################################

all	:	$(OUT)
clean	:
		/bin/rm -f $(OBJS) $(OUT)

$(OUT)	:	$(OBJS)
		/bin/rm -f $(OUT)
		if [ ! -d $(OUTDIR) ]; then mkdir -p $(OUTDIR); fi
		$(CC) $(FLAGS)  $(INCLUDEDIR) $(ROOTCFLAGS) $(ROOTLIBS) -o $(OUT) $(OBJS) $(DEPLIBS)

$(OBJS)	:	$(INCLUDES) Makefile

%.o	:	%.cpp
		$(CC) $(INCLUDEDIR) $(ROOTCFLAGS) -c -o $@ $<

