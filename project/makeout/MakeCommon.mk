#*******************************************************************************************
# Makefile for building the source code
#
# This is free, public domain software and there is NO WARRANTY.
# No restriction on use. You can use, modify and redistribute it for
# personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
#
# Sheldon Patterson
# ********************************************************************************************

DEP  = $(OBJ:.o=.d)

# --- Convert .out to .hex ---
$(BIN_OUT_DIR)/$(TARGET).bin: $(TARGET).out
	@echo $(MSG_CONVERT)
	@$(ELF) $(BIN_OUT_DIR)/$(TARGET).out $(BIN_OUT_DIR)/$(TARGET).bin --bin --silent
	
# --- Main Linker ---
$(TARGET).out : $(OBJ) $(LKR_FILE)
	@echo $(MSG_LINKER)
	@$(LR) $(OBJ) -o $(OUT_DIR)/$(TARGET).out $(LFLAGS)
 
# --- Compiler ---
%.o : %.c
	@echo $(MSG_COMPILER)
	@$(CC) $(SILENT) $< $(CFLAGS) --dependencies=m $*.d
	-@$(SED) -i "s/.*$@:/$@:/g" $*.d

# --- Assembler ---
%.o : %.s
	@echo $(MSG_ASSEMBLER)
	@$(AR) -S $< -O$(OUT_DIR)/ $(AFLAGS)

# --- Cleaner ---
clean:
	@echo .
	@echo .
	-@$(RM) *.o *.d *.i *.lst *.out *.sim *.map *.lob *.lnt *.log *.txt 2> NUL
#	@echo $(MSG_CLEANER)
        
clean_lint:
	@echo .
	@echo .
	-@$(RM) *.lob *.lnt *.txt 2> NUL
#	@echo $(MSG_CLEAN_LINT)


-include $(DEP)

.PHONY : all main $(LKR_FILE) test begin build coff debug eep elf end extcoff finish gccversion \
         gdb-config hex lss program sizeafter sizebefore sym \

