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

DEFS = -DUNIT_TESTING=1
 
CFLAGS = $(DEFS) \
        --no_cse --no_unroll --no_inline --no_code_motion --no_tbaa --no_clustering \
	--no_scheduling --debug --endian=little --cpu=$(CPU) -e --fpu=$(FPU) --diag_suppress=Pa050 \
        --dlib_config $(IAR_INC_DIR)/DLib_Config_Full.h $(PREINC_DIR) -Ol
        
TARGET = $(UNIT_TARGET)
OBJ = $(UNIT_OBJ)
DEP = $(UNIT_DEP)

all main test: ../$(TARGET).log

../$(TARGET).log : $(TARGET).out
	@echo $(MSG_UNIT_TEST)
	@$(CSPY) $(CSPY_DLL) $(TARGET).out $(CSPY_ARGS) > ../$(TARGET).log
	@type "..\$(TARGET).log"

include ../MakeCommon.mk

-include $(DEP)

