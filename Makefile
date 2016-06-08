.EXPORT_ALL_VARIABLES:

.PHONY: clean all docs

ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs)
INCLUDES      = -I./

CPP             = g++
CFLAGS		= -Wall -fPIC $(ROOTCFLAGS) $(INCLUDES) -Wl,-rpath=./

LIBS = $(ROOTLIBS) -L./ -lCommandLineInterface

O_FILES = daq.o dpp.o TSsort.o
all: libs sorter
libs: libCommandLineInterface.so

define EXE_COMMANDS
@echo "Compiling $@"
$(CPP) $(CFLAGS) $(LIBS) $< $(filter %.o,$^) -o $@
endef

sorter: sorter.cpp $(O_FILES) | libs
	$(EXE_COMMANDS)

define LIB_COMMANDS
@echo "Making $@"
@$(CPP) $(CFLAGS) -fPIC -shared -Wl,-soname,$@ -o $@ $^ -lc
endef

lib%.so: %.o
	$(LIB_COMMANDS)

-include $(wildcard *.d)

%.o: %.cpp %.h
	@echo "Compiling $@"
	@$(CPP) $(CFLAGS) -c $< -o $@

CommandLineInterface.o: CommandLineInterface.cpp CommandLineInterface.h
	@echo "Compiling $@"
	@$(CPP) $(CFLAGS) -c $< -o $@


clean:
	@echo Cleaning up
	@rm -f *.o lib*.so
	@rm -f sorter

