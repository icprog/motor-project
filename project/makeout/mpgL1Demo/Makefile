#*******************************************************************************************
# Makefile for building the source code
#
# This is free, public domain software and there is NO WARRANTY.
# No restriction on use. You can use, modify and redistribute it for
# personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
#
# Sheldon Patterson
# ********************************************************************************************

PROJ_DIR = ../../..
include ../MakeInclude.mk

DEFS = $(DEFS_COMMON)
CFLAGS = $(DEFS) $(CFLAGS_COMMON)
TARGET = $(MAIN_TARGET)
OBJ = $(BIN_OBJ)
DEP = $(BIN_DEP)
LINT_TARGET = $(MAIN_TARGET)_lint.log
OUT_DIR = $(BIN_OUT_DIR)

all: $(LINT_OUT)

# --- Lint ---
$(LINT_OUT) : $(LNT) $(BIN_OUT_DIR)/$(TARGET).bin
	@echo $(MSG_LINTER)
	@$(FIND) *.lnt -size +200c | $(XARGS) $(CAT) > $(LINT_OUT)
	@$(LINT) -b $(LINT_FILE) -zero +os($(LINT_OUT)) *.lob
	@$(SED) -i "s/^--- Object.*//g" $(LINT_OUT)
	@$(SED) -i "/./,/^$$/!d" $(LINT_OUT)
	@echo $(MSG_LINT_OUT)
	type "..\$(TARGET)_lint.log"

include ../MakeCommon.mk

## --- Linter ---
%.lnt : %.o
	@echo $(MSG_FILE_LINTER)
	@$(LINT) -b $(LINT_FILE) -u -zero -os($*.lnt) $(PREINC_DIR) $(DEFS) -header($(LINT_PDEF)) $(<:.o=.c)  -oo($*.lob)

-include $(DEP)

