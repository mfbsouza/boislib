# SPDX-License-Identifier: MIT
#
# This file is part of boislib,
# a Collection of portable libraries to extended the C ecosystem.

# project name
PROJECT = boislib

# paths
TESTDIR  ?= ./tests
SRCDIR   ?= ./
INCDIR   ?= ./
BUILDDIR := ./build
DBGDIR   := $(BUILDDIR)/debug
RELDIR   := $(BUILDDIR)/release

# install path
DESTDIR    ?=
USRDIR     := /usr/local
LIBDIR     := $(DESTDIR)$(USRDIR)/lib
HEADERDIR  := $(DESTDIR)$(USRDIR)/include

# compiler
CROSS-COMPILE ?=
CC   ?= $(CROSS-COMPILE)gcc
CXX  ?= $(CROSS-COMPILE)g++
AR   ?= $(CROSS-COMPILE)ar

# compiler and linker flags
CFLAGS   := -Wall -I $(INCDIR) -MMD -MP
CXXFLAGS := -Wall -I $(INCDIR) -MMD -MP
LDFLAGS  ?=

CXXTESTFLAGS := -Wall -I $(INCDIR) -MMD -MP
LDTESTFLAGS  ?= -lgtest -lgtest_main -lpthread

# configure for release or debug
ifeq ($(RELEASE),1)
	EXEDIR    := $(RELDIR)
	OBJDIR    := $(RELDIR)/obj
	CFLAGS    += -O2 -DNDEBUG
	CXXFLAGS  += -O2 -DNDEBUG
else
	EXEDIR    := $(DBGDIR)
	OBJDIR    := $(DBGDIR)/obj
	CFLAGS    += -ggdb3 -O0 -DDEBUG
	CXXFLAGS  += -ggdb3 -O0 -DDEBUG
endif

# sources to compile
ALLCSRCS   ?= $(shell find $(SRCDIR) -path $(TESTDIR) -prune -o -type f -name "*.c" -print)
ALLCXXSRCS ?= $(shell find $(SRCDIR) -path $(TESTDIR) -prune -o -type f -name "*.cpp" -print)
TESTSRCS   ?= $(shell find $(TESTDIR) -type f -name "*.cpp")

# set the linker to g++ if there is any c++ source code
ifeq ($(ALLCXXSRCS),)
	LD := $(CROSS-COMPILE)gcc
else
	LD := $(CROSS-COMPILE)g++
endif

# objects settings
COBJS    := $(addprefix $(OBJDIR)/, $(notdir $(ALLCSRCS:.c=.o)))
CXXOBJS  := $(addprefix $(OBJDIR)/, $(notdir $(ALLCXXSRCS:.cpp=.o)))
TESTOBJS := $(addprefix $(OBJDIR)/, $(notdir $(TESTSRCS:.cpp=.o)))
OBJS     := $(COBJS) $(CXXOBJS)
DEPS     := $(OBJS:.o=.d) $(TESTOBJS:.o=.d) 

# paths where to search for sources
SRCPATHS := $(sort $(dir $(ALLCSRCS)) $(dir $(ALLCXXSRCS)) $(dir $(TESTSRCS)))
VPATH     = $(SRCPATHS)

#output
SHARED := $(EXEDIR)/$(PROJECT).so
STATIC := $(EXEDIR)/$(PROJECT).a
HEADERSSRC  ?= $(shell find $(SRCDIR) -path $(TESTDIR) -prune -o -type f -name "*.h" -print)

# targets
.PHONY: all tests run_tests coverage report format clean install

all: $(OBJDIR) $(OBJS) $(SHARED) $(STATIC)

# targets for the directories
$(OBJDIR):
ifeq ($(VERBOSE),1)
	mkdir -p $(OBJDIR)
else
	@mkdir -p $(OBJDIR)
endif

# target for c objects
$(COBJS) : $(OBJDIR)/%.o : %.c
ifeq ($(VERBOSE),1)
	$(CC) -c $(CFLAGS) $< -o $@
else
	@echo -n -e '[CC]\t$<\n'
	@$(CC) -c $(CFLAGS) $< -o $@
endif

# target for cpp objects
$(CXXOBJS) : $(OBJDIR)/%.o : %.cpp
ifeq ($(VERBOSE),1)
	$(CXX) -c $(CXXFLAGS) $< -o $@
else
	@echo -n -e '[CXX]\t$<\n'
	@$(CXX) -c $(CXXFLAGS) $< -o $@
endif

# target for shared library
$(SHARED): $(OBJS)
ifeq ($(VERBOSE),1)
	$(LD) $(LDFLAGS) -shared $(OBJS) -o $@
else
	@echo -n -e '[LD]\t./$@\n'
	@$(LD) $(LDFLAGS) -shared $(OBJS) -o $@
endif

# target for static library
$(STATIC): $(OBJS)
ifeq ($(VERBOSE),1)
	$(AR) rcs $@ $(OBJS)
else
	@echo -n -e '[AR]\t./$@\n'
	@$(AR) rcs $@ $(OBJS)
endif

# target for install
install: all
	mkdir -p $(LIBDIR)
	mkdir -p $(HEADERDIR)/$(PROJECT)
	install -m 755 $(SHARED) $(LIBDIR)
	install -m 755 $(STATIC) $(LIBDIR)
	install -m 644 $(HEADERSSRC) $(HEADERDIR)/$(PROJECT)
	ldconfig $(LIBDIR)

# target for test cpp objects
$(TESTOBJS) : $(OBJDIR)/%.o : %.cpp
ifeq ($(VERBOSE),1)
	$(CXX) -c $(CXXTESTFLAGS) $< -o $@
else
	@echo -n -e '[CXX]\t$<\n'
	@$(CXX) -c $(CXXTESTFLAGS) $< -o $@
endif

tests: $(OBJDIR) $(OBJS) $(TESTOBJS)
ifeq ($(VERBOSE),1)
	$(CXX) $(LDTESTFLAGS) $(OBJS) $(TESTOBJS) -o $(EXEDIR)/$(PROJECT)_tests
else
	@echo -n -e '[LD]\t$(EXEDIR)/$(PROJECT)_tests\n'
	@$(CXX) $(LDTESTFLAGS) $(OBJS) $(TESTOBJS) -o $(EXEDIR)/$(PROJECT)_tests 
endif

run_tests: tests
	$(EXEDIR)/$(PROJECT)_tests

run_tests_no_output: tests
	@$(EXEDIR)/$(PROJECT)_tests --gtest_brief=1 > /dev/null

coverage: CFLAGS += --coverage
coverage: CXXFLAGS += --coverage
coverage: LDTESTFLAGS += --coverage
coverage: clean run_tests_no_output
	@echo
	@lcov --capture --directory $(OBJDIR) \
		--output-file $(BUILDDIR)/coverage.info

report: coverage
	@genhtml $(BUILDDIR)/coverage.info \
		--output-directory $(BUILDDIR)/report > /dev/null
	@echo
	@echo -n "Written coverage report to $(BUILDDIR)/coverage/index.html"
	@echo


format:
	find . \
		-regex '.*\.\(cpp\|hpp\|cc\|cxx\|c\|h\)' \
		-exec clang-format -style=file -i {} \;

clean:
	rm -rf $(BUILDDIR)

# Include the dependency files, should be the last in the makefile
-include $(DEPS)
