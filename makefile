# Generic Makefile for compiling a simple executable.
#

PROJECT_NAME = faltu

SRCDIR = src
OBJDIR = build
BINDIR = bin

TARGET := main
SRCEXT := c

#SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
#
SRC = main.c libPFF/diskio.c libPFF/pff.c libMassStorageHost/MassStoreCommands.c libMassStorageHost/print_struct.c

EXTRAINCDIRS = src src/libMassStorageHost src/libPFF

LIBUSB_DIR = /usr/local/lib

#
CFLAGS = -g -Wall -Os
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
CFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))

#
LDFLAGS = -Wall -L$(LIBUSB_DIR) -lusb-1.0
LDFLAGS += -Wl,--gc-sections

#OBJECTS = $(patsubst %,$(OBJDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
OBJ = $(SRC:%.c=$(OBJDIR)/%.o)
DEPS = $(OBJECTS:.o=.deps)
CC = gcc
SIZE = size


all: msg_start make_dir $(BINDIR)/$(TARGET) msg_end 

$(BINDIR)/$(TARGET): $(OBJ)
	@echo "\nLinking...";
	@$(CC) $^ -o $@ $(LDFLAGS)
	@echo "\nCalculating Size of executable file...";
	@$(SIZE) $@

$(OBJDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@echo
	@echo "Compiling... $<";
	@$(CC) $(CFLAGS) -MD -MF $(@:.o=.deps) -c -o $@ $<

clean:
	@echo "\nCleaning Project $(PROJECT_NAME)... \n";
	@$(RM) -r $(OBJDIR) $(BINDIR)
	@echo "Removed Build files and executable file.";

make_dir:
	@mkdir -p $(OBJDIR) $(OBJDIR)/libPFF $(OBJDIR)/libMassStorageHost
	@mkdir -p $(BINDIR)

msg_start:
	@echo
	@echo "Welcome to Makefile of the project $(PROJECT_NAME).";
	@echo "Building project, Please wait... ";

msg_end:
	@echo
	@echo "Project $(PROJECT_NAME) Build Complete.";
	@echo "Thank you.";

-include $(DEPS)
.PHONY: clean