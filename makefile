# Generic Makefile for compiling a simple executable.
#
CC := gcc
SRCDIR = src
BUILDDIR = build
TARGET := main
SRCEXT := c
#SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
#
SOURCES = main libPFF/diskio libPFF/pff libMassStorageHost/MassStoreCommands

EXTRAINCDIRS = src src/libMassStorageHost src/libPFF

#
CFLAGS = -g -Wall
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
CFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))

#
LDFLAGS = -Wall -L/usr/local/lib -lusb-1.0
LDFLAGS += -Wl,--gc-sections

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