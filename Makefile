# Change to 0 for release version
DEBUG   := 0
PROFILE := 0

# Version
VERSION := 0.0.1

# See http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
# for the template used to start this file
SDIR              := src

CORE_DIR          := $(SDIR)/core

CORE_ODIR         := $(CORE_DIR)/.objs

CORE_DEP_DIR      := $(CORE_DIR)/.deps

# Dir where the library will be placed after building
LIB_DIR := lib

# The aplication generated
LIBNAME := $(LIB_DIR)/libgrackle.so#.$(VERSION)

# -- TODO: customize the list below for your project ---
# List of source .c files used with the project
CORE_SRCS := $(wildcard $(CORE_DIR)/*.cpp)

# Replace .c extension on SRCS to get objfiles using gnu make pattern rules and substitution references.
# See https://www.gnu.org/software/make/manual/html_node/Pattern-Intro.html#Pattern-Intro for pattern rules and 
# https://www.gnu.org/software/make/manual/html_node/Substitution-Refs.html#Substitution-Refs for substitution references overview
CORE_OBJFILES := $(CORE_SRCS:$(CORE_DIR)/%.cpp=$(CORE_ODIR)/%.o)

# Add all warnings/errors to cflags default.  This is not required but is a best practice
# -fPIC is neccessary compiling object code to create a shared library
CFLAGS := -std=c++11 -Wall -Werror -fPIC

# Build the app you've specified in APPNAME for the "all" or "default" target
all: dirs debug profile $(LIBNAME)

dirs:
	@mkdir -p $(CORE_ODIR)
	
	@mkdir -p $(CORE_DEP_DIR)

	@mkdir -p $(LIB_DIR)


debug:
ifeq ($(DEBUG), 0)
CFLAGS += -O2 -DNDEBUG
else
CFLAGS += -g -O0
endif


profile:
ifeq ($(PROFILE), 1)
CFLAGS  += -pg
LDFLAGS += -pg
endif


# Remove all build intermediates and output file
clean: ; rm -rf $(LIBNAME) \
	$(CORE_DEP_DIR)/*.d \
	$(CORE_ODIR)/*.o
	@echo Grackle cleaned


# Build the application by running the link step with all objfile inputs
$(LIBNAME): $(CORE_OBJFILES)
	$(CXX) -shared -fPIC $(CORE_OBJFILES) -o $@
	@echo libgrackle built successfully


# The below content is from  http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
# with the following changes:
#   1) Added comments
#   2) Removed TARGET_ARCH from COMPILE.c since it's no longer listed in the [default rules](https://www.gnu.org/software/make/manual/html_node/Catalogue-of-Rules.html#Catalogue-of-Rules) and [isn't documented](https://lists.gnu.org/archive/html/help-make/2010-06/msg00005.html)
# Original content below is:
# Copyright © 1997-2019 Paul D. Smith Verbatim copying and distribution is permitted in any medium, provided this notice is preserved.

# Flags passed to gcc to automatically build dependencies when compiling
# See https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html for detail about variable names
# $@ references the target file of the rule and will be "main.o" when compiling "main.c"
# $* references the stem of the rule, and will be "main" when target is "main.o"
CORE_DEP_FLAGS = -MT $@ -MMD -MP -MF $(CORE_DEP_DIR)/$*.d

# Rules for compiling a C file, including DEPFLAGS along with Implicit GCC variables.
# See https://www.gnu.org/software/make/manual/html_node/Implicit-Variables.html
# and see https://www.gnu.org/software/make/manual/html_node/Catalogue-of-Rules.html#Catalogue-of-Rules
# for the default c rule
CORE_COMPILE.cpp = $(CXX) $(CORE_DEP_FLAGS) $(CFLAGS) $(CPPFLAGS) -c

# Delete the built-in rules for building object files from .c files
%.o: %.cpp
# Define a rule to build object files based on .c or dependency files by making the associated dependency file
# a prerequisite of the target.  Make the DEPDIR an order only prerequisite of the target, so it will be created when needed, meaning
# the targets won't get rebuilt when the timestamp on DEPDIR changes
# See https://www.gnu.org/software/make/manual/html_node/Prerequisite-Types.html for order only prerequesites overview.
$(CORE_ODIR)/%.o: $(CORE_DIR)/%.cpp $(CORE_DEP_DIR)/%.d | $(CORE_DEP_DIR)
	$(CORE_COMPILE.cpp) $(OUTPUT_OPTION) $<


# Use pattern rules to build a list of DEPFILES
CORE_DEPFILES := $(CORE_SRCS:$(CORE_DIR)/%.cpp=$(CORE_DEP_DIR)/%.d)

# Mention each of the dependency files as a target, so make won't fail if the file doesn't exist
$(CORE_DEPFILES):


# Include all dependency files which exist, to include the relevant targets.
# See https://www.gnu.org/software/make/manual/html_node/Wildcard-Function.html for wildcard function documentation
include $(wildcard $(CORE_DEPFILES))
