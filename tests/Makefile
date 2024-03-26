# binary name
PROJECT := boislib
NAME    := $(PROJECT)_tests

# sources and build paths
TESTDIR  ?= ./
SRCDIR   ?= ../src
INCDIR   ?= ../src
BUILDDIR := ./build
DBGDIR   := $(BUILDDIR)/debug
RELDIR   := $(BUILDDIR)/release

# compiler and linker
CROSS-COMPILE ?=
CC   ?= $(CROSS-COMPILE)gcc
CXX  ?= $(CROSS-COMPILE)g++
LD   := $(CROSS-COMPILE)g++

# compiler and linker flags
CFLAGS   := -Wall -I $(INCDIR) -MMD -MP
CXXFLAGS := -Wall -I $(INCDIR) -MMD -MP
LDFLAGS  ?= -lgtest -lgtest_main -lpthread

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
ALLCSRCS   ?= $(shell find $(SRCDIR) -type f -name "*.c")
ALLCXXSRCS ?= $(shell find $(TESTDIR) -type f -name "*.cpp")

# objects settings
COBJS   := $(addprefix $(OBJDIR)/, $(notdir $(ALLCSRCS:.c=.o)))
CXXOBJS := $(addprefix $(OBJDIR)/, $(notdir $(ALLCXXSRCS:.cpp=.o)))
OBJS    := $(COBJS) $(CXXOBJS)
DEPS    := $(OBJS:.o=.d)

# paths where to search for sources
SRCPATHS := $(sort $(dir $(ALLCSRCS)) $(dir $(ALLCXXSRCS)))
VPATH     = $(SRCPATHS)

# output
OUTFILES := $(EXEDIR)/$(NAME)

# targets
.PHONY: all clean run_tests coverage

all: $(OBJDIR) $(EXEDIR) $(OBJS) $(OUTFILES)

# targets for the directories
$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(EXEDIR):
	@mkdir -p $(EXEDIR)

# target for c objects
$(COBJS) : $(OBJDIR)/%.o : %.c
ifeq ($(VERBOSE),1)
	$(CC) -c $(CFLAGS) $< -o $@
else
	@echo -n -e '[CC]\t./$<\n'
	@$(CC) -c $(CFLAGS) $< -o $@
endif

# target for cpp objects
$(CXXOBJS) : $(OBJDIR)/%.o : %.cpp
ifeq ($(VERBOSE),1)
	$(CXX) -c $(CXXFLAGS) $< -o $@
else
	@echo -n -e '[CXX]\t./$<\n'
	@$(CXX) -c $(CXXFLAGS) $< -o $@
endif

# target for ELF file
$(EXEDIR)/$(NAME): $(OBJS)
ifeq ($(VERBOSE),1)
	$(LD) $(LDFLAGS) $(OBJS) -o $@
else
	@echo -n -e '[LD]\t./$@\n'
	@$(LD) $(LDFLAGS) $(OBJS) -o $@
endif

run_tests: all
	$(EXEDIR)/$(PROJECT)_tests

run_tests_no_output: all
	@$(EXEDIR)/$(PROJECT)_tests --gtest_brief=1 > /dev/null

coverage: CFLAGS += --coverage
coverage: LDFLAGS += --coverage
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

clean:
	rm -rf $(BUILDDIR)

# Include the dependency files, should be the last in the makefile
-include $(DEPS)
