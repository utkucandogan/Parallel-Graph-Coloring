OUT ?= pgc.elf

CXXFLAGS ?= -Wall -O0 -g -std=c++17

CXX := mpic++

SRC_DIR  := parallel_src
INC_DIR  := parallel_src
SRCS_C   := $(wildcard $(SRC_DIR)/*.c)
SRCS_CPP := $(wildcard $(SRC_DIR)/*.cpp)
SRCS     := $(SRCS_C) $(SRCS_CPP)

OBJ_DIR  := obj
OBJS_C   := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS_C))
OBJS_CPP := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS_CPP))
OBJS     := $(OBJS_C) $(OBJS_CPP)


default: all

clean:
	rm -rf $(OBJ_DIR) $(OUT)

all: release

debug: CXXFLAGS += -O0 -g -DDEBUG
debug: executable

broadcast: CXXFLAGS += -O0 -g -Dbroadcast
broadcast: executable

release: CXXFLAGS += -O3
release: executable

executable: $(OUT)

./$(OUT): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

.PHONY: clean
.PRECIOUS: ./$(OUT) $(OBJ_DIR)/%.o
