APPLICATION := custom_pass

# Environment
CC := clang-14
CXX := clang++-14

# Pass
PASS_DIR := ./pass
STATIC_PASS_DIR := ./pass/static
DYNAMIC_PASS_DIR := ./pass/dynamic
PASS_BIN_DIR := ./pass/dynamic/bin

PASS_NAME := VisualDumpPass
PASS_SO := $(STATIC_PASS_DIR)/$(addprefix lib, $(PASS_NAME).so)

PASS_SRC := $(wildcard $(addsuffix /*.cpp, $(DYNAMIC_PASS_DIR)))
PASS_OBJ := $(addprefix $(PASS_BIN_DIR)/, $(patsubst %.cpp, %.o, $(notdir $(PASS_SRC))))

# Project
INC_DIRS := pass/include
BIN_DIR := bin
BUILD_DIR := build
DUMP_DIR := dump
APP_BUILD := $(addprefix $(BUILD_DIR)/, $(APPLICATION))

SRC_DIRS := src src/calc
VPATH += $(SRC_DIRS)

SRC := $(wildcard $(addsuffix /*.cpp, $(SRC_DIRS)))
OBJ := $(addprefix $(BIN_DIR)/, $(patsubst %.cpp, %.o, $(notdir $(SRC))))

# Flags
CMAKE_FLAGS := -DCMAKE_CXX_COMPILER=$(CXX) -DCMAKE_C_COMPILER=$(CC)
LD_FLAGS := -pie -pthread -flto
CXX_FLAGS := -Weverything -ggdb3 -O0 -std=c++14 $(addprefix -I, $(INC_DIRS)) \
             -flegacy-pass-manager -Xclang -load -Xclang $(PASS_SO)

# Usage:
# "make all"  to build the whole project
# "make pass" to build the pass only
all: prepare pass $(APP_BUILD) png

$(APP_BUILD): $(OBJ) $(PASS_OBJ)
	@$(CXX) $^ -o $@ $(LD_FLAGS)

$(BIN_DIR)/%.o: %.cpp
	@$(CXX) $< -c -MD -o $@ $(CXX_FLAGS)

$(PASS_BIN_DIR)/%.o: $(DYNAMIC_PASS_DIR)/%.cpp
	@$(CXX) $< -c -MD -o $@ $(addprefix -I, $(INC_DIRS))

$(PASS_SO): $(wildcard $(addsuffix /*.cpp, $(STATIC_PASS_DIR)))
	@cmake -S $(STATIC_PASS_DIR) -B $(STATIC_PASS_DIR) $(CMAKE_FLAGS)
	@cmake -S $(PASS_DIR) -B $(PASS_DIR) $(CMAKE_FLAGS)
	@make  -C $(PASS_DIR)

-include $(wildcard $(BIN_DIR)/*.d)

# Pass
.PHONY: pass pass_static pass_dynamic

pass: prepare pass_static pass_dynamic

pass_static: $(PASS_SO)

pass_dynamic: $(PASS_OBJ)

# General
.PHONY: all run gdb valgrind prepare clean info png

run: all
	@$(APP_BUILD)

gdb: all
	@gdb $(APP_BUILD)

valgrind: all
	@valgrind --leak-check=full $(APP_BUILD)

prepare:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(PASS_BIN_DIR)

info:
	@echo [*] OBJ: $(OBJ)
	@echo [*] SRC: $(SRC)

png:
	@mkdir -p $(DUMP_DIR)
	@dot -Tpng dump.dot > $(DUMP_DIR)/dump.png

clean:
	@rm -rf $(BIN_DIR)
	@rm -rf $(BUILD_DIR)
	@rm -rf $(PASS_BIN_DIR)
	@rm -rf $(DUMP_DIR)

	@make clean -C $(PASS_DIR)
	@make clean -C $(STATIC_PASS_DIR)
