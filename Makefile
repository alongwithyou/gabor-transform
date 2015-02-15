#------------------------------------------------
# Generic Makefile
#
# Author : yanick.rochon@gmail.com
# Date   : 2011-08-10
#
# Changelog :
#   04 Nov 2010 - first version
#   10 Aug 2011 - added structure : sources, objects, binaries
#                 thanks to http://stackoverflow.com/users/128940/beta
#   02 Feb 2015 - modified for Masters thesis
# ------------------------------------------------

# Executable name
TARGET   = gabor

CC       = gcc

OPTIMIZE = -O0

# -std=c99           : Set c99 as the language standard
# -Wall              : Enable a large set of compiler warnings
# -pedantic          : Ensure strict coherence with the standard set
# -ftrapv            : Generates additional code that kills on integer overflow. Limits optimization.
# -O0                : Turn off optimization
# -Og                : Turn on optimizations that do not interfere with debugging
# -Werror            : Makes warnings errors
# -Wdouble-promotion : Makes implicit promotion to a double a warning


CFLAGS   = $(OPTIMIZE) -g -std=c99 -pedantic -Wall -Wdouble-promotion

LINKER   = $(CC) -o
# linking flags here
LFLAGS   = -g -std=c99 -pedantic -Wall -Wdouble-promotion
LIBS     = -lm -lfreeimage -lfftw3

# Change these to set the proper directories where each files shoould be
# Should eventually be "src", "obj", "bin"
SRCDIR   = .
OBJDIR   = .
BINDIR   = ~

# These sort files into the proper spots
SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
rm       = rm -f

# Rule for linking object files
$(BINDIR)/$(TARGET): $(OBJECTS)
	$(LINKER) $@ $(LFLAGS) $(OBJECTS) $(LIBS)

# Rule for making object files
$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to clean subdirectories
clean:
	$(rm) $(OBJECTS)
	$(rm) $(BINDIR)/$(TARGET)
