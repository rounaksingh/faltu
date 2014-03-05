# Generic Makefile for compiling a simple executable.
#
SRCDIR = src
OBJDIR = build

TARGET := main
SRCEXT := c

#SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
#
SRC = main.c libPFF/diskio.c libPFF/pff.c libMassStorageHost/MassStoreCommands.c libMassStorageHost/print_struct.c

EXTRAINCDIRS = src src/libMassStorageHost src/libPFF

#
CFLAGS = -g -Wall
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
CFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))

#
LDFLAGS = -Wall -L/usr/local/lib -lusb-1.0
LDFLAGS += -Wl,--gc-sections

#OBJECTS = $(patsubst %,$(OBJDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
OBJ = $(SRC:%.c=$(OBJDIR)/%.o)
DEPS = $(OBJECTS:.o=.deps)
CC = gcc

$(TARGET): $(OBJ)
	@echo " Linking...";
	$(CC) $^ -o $(TARGET) $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(OBJDIR) $(OBJDIR)/libPFF $(OBJDIR)/libMassStorageHost
#	@echo " CC $<"; 
	$(CC) $(CFLAGS) -MD -MF $(@:.o=.deps) -c -o $@ $<

clean:
	@echo " Cleaning..."; 
	$(RM) -r $(OBJDIR) $(TARGET)

-include $(DEPS)

.PHONY: clean