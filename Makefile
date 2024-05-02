#Variables
BVAL=0
AVAL=1

#Directories
MINGW_GCC_DIR = C:/MinGW
MINGW_GCC_BIN_DIR = $(MINGW_GCC_DIR)/bin
MINGW_GCC_INCLUDE_DIR = $(MINGW_GCC_DIR)/include
MINGW_GCC_LIB_DIR = $(MINGW_GCC_DIR)/lib

BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin

ARCH = architecture
CONFIG = config
BENCHMARKS = benchmarks

#Toolchain
GNUCC = $(MINGW_GCC_BIN_DIR)/gcc.exe
RM = rmdir
MK = mkdir

#Flags
# WFLAGS = -Wall -Wextra -Werror -Wshadow 
DFLAGS = -Og -g
CFLAGS = $(addprefix -I , $(MINGW_GCC_INCLUDE_DIR)) $(WFLAGS) $(DFLAGS)
LDFLAGS = $(addprefix -L , $(MINGW_GCC_LIB_DIR))
BENCHMARKFLAGS = -D BENCHMARK=$(BVAL)
ARCHFLAGS = -D ARCH=$(AVAL)

#Files
TARGET = $(BIN_DIR)/subleq.exe

SOURCES_WITH_HEADERS = $(ARCH)\arch.c
						
SOURCES = $(SOURCES_WITH_HEADERS)

BENCHMARKS_HEADERS = $(BENCHMARKS)/fibo.h
CONFIG_HEADERS= $(CONFIG)/config.h
HEADERS = $(SOURCES_WITH_HEADERS:.c=.h) $(BENCHMARKS_HEADERS) $(CONFIG_HEADERS)

OBJECT_NAMES = $(SOURCES:.c=.o) $(SOURCES_WITH_HEADERS:.c=.o)
OBJECTS = $(patsubst %, $(OBJ_DIR)/%,$(OBJECT_NAMES))

STRUCTURE_DIRS = $(sort $(subst /,\,$(dir $(OBJECTS)))) $(subst /,\,$(BIN_DIR))

#Linking
$(TARGET): $(OBJECTS) $(HEADERS)
	$(GNUCC) $(LDFLAGS) $(BENCHMARKFLAGS) $(ARCHFLAGS) -o $@ $^

#Compiling
$(OBJ_DIR)/%.o: %.c
	$(GNUCC) $(CFLAGS) $(BENCHMARKFLAGS) $(ARCHFLAGS) -c -o $@ $^

#Phony
.PHONY: all clean dir run

all: $(TARGET)

clean: 
	@$(RM) build /s /q

dir:
	@$(MK) $(STRUCTURE_DIRS)

run:
	$(TARGET)