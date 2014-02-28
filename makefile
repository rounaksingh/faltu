# Generic Makefile for compiling a simple executable.
 
CC := gcc
SRCDIR := src
BUILDDIR := build
CFLAGS := -g -Wall
LDFLAGS := -Wall -L/usr/local/lib -lusb-1.0
TARGET := main
 
SRCEXT := c
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
#SOURCES= main.c diskio.c pff.c
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
DEPS := $(OBJECTS:.o=.deps)
 
$(TARGET): $(OBJECTS)
	@echo " Linking...";
	$(CC) $^ -o $(TARGET) $(LDFLAGS)
 
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
# @echo " CC $<"; 
	$(CC) $(CFLAGS) -MD -MF $(@:.o=.deps) -c -o $@ $<
 
clean:
	@echo " Cleaning..."; $(RM) -r $(BUILDDIR) $(TARGET)
 
-include $(DEPS)
 
.PHONY: clean