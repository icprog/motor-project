#*******************************************************************************************
# Makefile for building the source code
#
# This is free, public domain software and there is NO WARRANTY.
# No restriction on use. You can use, modify and redistribute it for
# personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
#
# Sheldon Patterson
# ********************************************************************************************

PROJ_DIR = ../..
include MakeInclude.mk

export MAIN_TARGET = mpgL1Demo
export UNIT_TARGET = unitTests
SUBDIRS = $(MAIN_TARGET) $(UNIT_TARGET)

all: pre-build $(SUBDIRS)
main: pre-build $(MAIN_TARGET)
test: pre-build $(UNIT_TARGET)

pre-build:
	$(TOUCH) $(LIB_VER_DIR)/version.c

all: $(SUBDIRS)

$(SUBDIRS) : 
	@echo $(MSG_MAKEFILE)
	@$(MAKE) -C $@ -W $(MAKEFLAGS) $(MAKECMDGOALS) -s

.PHONY: subdirs $(SUBDIRS)    


# --- Cleaner ---
clean: $(SUBDIRS)
	-@$(RM) "$(PROJ_DIR)\bin\*.*" 2> NUL
        
clean_lint: $(SUBDIRS)


.PHONY : all main test $(LKR_FILE) begin build coff debug eep elf end extcoff finish gccversion \
         gdb-config hex lss program sizeafter sizebefore sym \

