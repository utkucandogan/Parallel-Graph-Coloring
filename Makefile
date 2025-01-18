OUT     ?= pgc
CCFLAGS ?= -Wall
MODE    ?= parallel


ifeq ($(OS), Windows_NT)
	OUT_EXT ?= exe
	CCFLAGS += -I"C:\Program Files (x86)\Microsoft SDKs\MPI\Include" -L"C:\Program Files (x86)\Microsoft SDKs\MPI\Lib\x64" -lmsmpi
	CC      := gcc
else
	OUT_EXT ?= elf
	CC      := mpicc
endif


COMM := common
ifeq ($(MODE), parallel)
	CODE := parallel
else
	CODE := serial
endif


SRC_DIR      := src
SRC_DIR_COMM := $(SRC_DIR)/$(COMM)
SRC_DIR_CODE := $(SRC_DIR)/$(CODE)

SRCS_COMM := $(wildcard $(SRC_DIR_COMM)/*.c) $(wildcard $(SRC_DIR_COMM)/**/*.c)
SRCS_CODE := $(wildcard $(SRC_DIR_CODE)/*.c) $(wildcard $(SRC_DIR_CODE)/**/*.c)

OBJ_DIR      := obj
OBJ_DIR_COMM := $(OBJ_DIR)/$(COMM)
OBJ_DIR_CODE := $(OBJ_DIR)/$(CODE)

OBJS_COMM := $(patsubst $(SRC_DIR_COMM)/%.c, $(OBJ_DIR_COMM)/%.o, $(SRCS_COMM))
OBJS_CODE := $(patsubst $(SRC_DIR_CODE)/%.c, $(OBJ_DIR_CODE)/%.o, $(SRCS_CODE))


OUT_EXE := $(OUT)_$(CODE).$(OUT_EXT)


default: all

clean:
	rm -rf $(OBJ_DIR) $(OUT_EXE)

all: release

debug: CCFLAGS += -O0 -g -DDEBUG
debug: executable

broadcast: CCFLAGS += -O3 -Dbroadcast
broadcast: executable

release: CCFLAGS += -O3
release: executable

executable: $(OUT_EXE)

$(OUT_EXE): $(OBJS_COMM) $(OBJS_CODE)
	$(CC) $(CCFLAGS) $^ -o $@


$(OBJ_DIR_COMM)/%.o: $(SRC_DIR_COMM)/%.c
	mkdir -p $(@D)
	$(CC) $(CCFLAGS) -I$(SRC_DIR_COMM) -c $< -o $@

$(OBJ_DIR_CODE)/%.o: $(SRC_DIR_CODE)/%.c
	mkdir -p $(@D)
	$(CC) $(CCFLAGS) -I$(SRC_DIR_COMM) -I$(SRC_DIR_CODE) -c $< -o $@


.PHONY: clean
.PRECIOUS: $(OUT_EXE) $(OBJS_COMM) $(OBJS_CODE)
