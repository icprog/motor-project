#*******************************************************************************************
# Makefile for building the source code
#
# This is free, public domain software and there is NO WARRANTY.
# No restriction on use. You can use, modify and redistribute it for
# personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
#
# Sheldon Patterson
# ********************************************************************************************

MAKEFLAGS += -r
.SUFFIXES:
.SUFFIXES: .c .o .d .asm .s .out .bin .hex .lob .lnt


CPU     = CORTEX-M3
FPU     = NONE

# --- Paths ---
OUT_DIR = "$(CURDIR)"
BIN_OUT_DIR = $(PROJ_DIR)/bin
RES_DIR = $(PROJ_DIR)/resources
LINT_DIR = $(RES_DIR)/pcLint
IAR_DIR = C:/Program Files/IAR Systems/Embedded Workbench 6.4/arm
IAR_INC_DIR = "$(IAR_DIR)/inc/c"
IAR_LIB_DIR = "$(IAR_DIR)/lib"
IAR_BIN_DIR = $(IAR_DIR)/bin
IAR_CBIN_DIR = $(IAR_DIR)/../common/bin
UTILS_DIR = $(RES_DIR)/utils
SRC_DIR = $(PROJ_DIR)/src
APP_DIR = $(SRC_DIR)/app
BSP_DIR = $(SRC_DIR)/bsp
CFG_DIR = $(SRC_DIR)/config
DRIVERS_DIR = $(SRC_DIR)/drivers
FILE_DIR = $(SRC_DIR)/file
LIB_DIR = $(SRC_DIR)/lib
LIB_UTILS_DIR = $(LIB_DIR)/utils
LIB_STRUCT_ROOT = $(LIB_DIR)/structures
LIB_STRUCT_DIR = $(LIB_STRUCT_ROOT)/src
LIB_VER_DIR = $(LIB_DIR)/version
LIB_FILE_DIR = $(LIB_DIR)/file
LIB_FAT_DIR = $(LIB_DIR)/FatFs
LIB_FAT_DIR2 = $(LIB_DIR)/FatFs/option
LIB_FILE_HOOKS_DIR = $(LIB_FILE_DIR)/hooks/FatFs
LIB_FILE_DISK_DIR = $(LIB_FILE_DIR)/disk
UTILS_SRC_DIR = $(SRC_DIR)/utils
EMB_DIR  = $(LIB_DIR)/embUnit
EMB_SRC_DIR = $(EMB_DIR)/embUnit
EMB_UI_DIR  = $(EMB_DIR)/textUi
UNIT_TEST_DIR = $(SRC_DIR)/unitTests
UNIT_TEST_STR_DIR = $(LIB_STRUCT_ROOT)/unitTests

VPATH = $(SRC_DIR):$(APP_DIR):$(BSP_DIR):$(CFG_DIR):$(DRIVERS_DIR):$(FILE_DIR)\
       :$(LIB_UTILS_DIR):$(LIB_STRUCT_DIR):$(LIB_VER_DIR):$(UNIT_TEST_STR_DIR)\
       :$(LIB_FILE_DIR):$(LIB_FAT_DIR):$(LIB_FAT_DIR2):$(LIB_FILE_HOOKS_DIR):$(LIB_FILE_DISK_DIR)\
       :$(UTILS_SRC_DIR):$(EMB_DIR):$(EMB_SRC_DIR):$(EMB_UI_DIR):$(UNIT_TEST_DIR)

# --- Files ---
LKR_FILE = $(PROJ_DIR)/project/sam3u2-flash.icf
LINT_FILE = $(PROJ_DIR)/project/pc-lint.lnt
LINT_PDEF = $(PROJ_DIR)/project/pre_defs.h
LINT_OUT = $(PROJ_DIR)/project/makeout/$(TARGET)_lint.log

# --- Executables ---
CC = "$(IAR_BIN_DIR)/iccarm.exe"
AR = "$(IAR_BIN_DIR)/iasmarm.exe"
LR = "$(IAR_BIN_DIR)/ilinkarm.exe"
ELF = "$(IAR_BIN_DIR)/ielftool.exe"
LINT = "$(LINT_DIR)/lint9edu.exe"
TOUCH = "$(UTILS_DIR)/touch.exe"
SED = "$(UTILS_DIR)/sed.exe"
FIND = "$(UTILS_DIR)/find.exe"
XARGS = "$(UTILS_DIR)/xargs.exe"
CAT = "$(UTILS_DIR)/cat.exe"
CSPY = "$(IAR_CBIN_DIR)/cspybat.exe"
CSPY_DLL = "$(IAR_BIN_DIR)/armproc.dll" "$(IAR_BIN_DIR)/armsim2.dll"

RM = del /f /Q


# ---- Messages ----
MSG_SPACE       = "<<<-------------------------------------->>>"
MSG_MAKEFILE    = "(-- $(@) Makefile: --)"
MSG_CONVERT     = "<<<-----  Converting $(TARGET).out to $(TARGET).hex  ------>>>"
MSG_LINKER      = "<<<-----  Linking $(TARGET):   ------>>>"
MSG_ASSEMBLER   = "<<<----- Assembling $(<F): ------>>>"
MSG_COMPILER    = "<<<----- Compiling $(<F):  ------>>>"
MSG_CLEANER     = "<<<----- Cleaning Project ---------->>>"
MSG_FILE_LINTER = "<<<----- Linting $(<:.r90=.c):    ------>>>"
MSG_LINTER      = "<<<---------- Linting Project ----------->>>"
MSG_LINT_OUT    = "Lint Output is in $(LINT_TARGET)"
MSG_IMG_GEN     = "<<<---------- Creating Upgrade Image ----------->>>"
MSG_CLEANER     = "<<<----- All project output files have been removed ----->>>>>"
MSG_CLEAN_LINT  = "<<<----- All lint files have been removed ----->>>>>"
MSG_UNIT_TEST   = "<<<----- Running Unit Tests ... ----->>>>>"


# --- Assembler/Compiler/Linker ---
PREINC_DIR  = -I$(SRC_DIR) \
              -I$(APP_DIR) \
              -I$(BSP_DIR) \
              -I$(CFG_DIR) \
              -I$(DRIVERS_DIR) \
              -I$(LIB_FILE_DIR) \
              -I$(LIB_UTILS_DIR) \
              -I$(LIB_STRUCT_DIR) \
              -I$(LIB_FAT_DIR) \
              -I$(LIB_FILE_HOOKS_DIR) \
              -I$(LIB_FILE_DISK_DIR) \
              -I$(LIB_VER_DIR) \
              -I$(IAR_INC_DIR) \
              -I$(UTILS_SRC_DIR) \
              -I$(EMB_DIR) \
              -I$(UNIT_TEST_DIR) \
              -I$(UNIT_TEST_STR_DIR) \
	      
DEFS_COMMON =

AFLAGS = -s+ -w+ -r "-M<>" -L -cAOM -i -B -t8 --cpu $(CPU) --fpu None
CFLAGS_COMMON = $(DEFS) -lcN "$(CURDIR)"/ --preprocess=c ./ --diag_suppress=Pa050 \
                --no_cse --no_unroll --no_inline --no_tbaa \
                --no_scheduling --debug --endian=little --cpu=$(CPU) -e --fpu=$(FPU) -Om \
                --dlib_config $(IAR_INC_DIR)/DLib_Config_Normal.h $(PREINC_DIR)

LFLAGS = --redirect __write=__write_buffered --map "$(OUT_DIR)/$(TARGET).map" \
         --config "$(LKR_FILE)" --semihosting --entry __iar_program_start \
         --merge_duplicate_sections --vfe --no_exceptions --inline
SILENT = --silent
CSPY_ARGS = --plugin "$(IAR_BIN_DIR)/armLibSupport.dll" --plugin "$(IAR_BIN_DIR)/armbat.dll" \
            --backend -B "--endian=little" "--cpu=$(CPU)" "--fpu=$(FPU)" "-p" \
            "$(IAR_DIR)/config/debugger/Atmel/ioat91sam3u2.ddf" "--semihosting" "--device=SAM3U2C"


# --- Source Files ---
#NO_LINT_SRC = board_cstartup_iar.c core_cm3.c irq.c ccsbcs.c ff.c time.c xgetdst.c
NO_LINT_SRC = board_cstartup_iar.c core_cm3.c irq.c ff.c time.c xgetdst.c

SRC = beeper.c \
      button.c \
      bsp.c \
      circularBuffer.c \
      demo.c \
      file.c \
      fileFat.c \
      fileSystem.c \
      futils.c \
      gpio.c \
      i2c.c \
      itemQ.c \
      lcd.c \
      led.c \
      music.c \
      rtc.c \
      sd.c \
      songTwinkleTwinkle.c \
      sysTick.c \
      textEditor.c \
      threadUtils.c \
      timer.c \
      usart.c \
      utils.c \
      version.c \
      wdt.c

EMB_SRC = AssertImpl.c \
          RepeatedTest.c \
          stdImpl.c \
          TestCaller.c \
          TestCase.c \
          TestResult.c \
          TestRunner.c \
          TestSuite.c \
          CompilerOutputter.c \
          TextOutputter.c \
          TextUIRunner.c \
          XMLOutputter.c
	  
UNIT_TEST_SRC = unitTests.c \
                circularBufferTest.c \
                itemQTest.c


ALL_SRC = $(NO_LINT_SRC) $(SRC) main.c
BIN_OBJA = $(ALL_SRC:.s=.o)
BIN_OBJ  = $(BIN_OBJA:.c=.o)
BIN_DEP  = $(BIN_OBJ:.o=.d)

UNIT_SRC = $(SRC) $(EMB_SRC) $(UNIT_TEST_SRC)
UNIT_OBJA1 = $(UNIT_SRC:.s=.o)
UNIT_OBJA2 = $(UNIT_OBJA1:.asm=.o)
UNIT_OBJ   = $(UNIT_OBJA2:.c=.o)
UNIT_DEP  = $(UNIT_OBJ:.o=.d)

LNT_SRC = $(SRC) main.c
LNT  = $(LNT_SRC:.c=.lnt)

