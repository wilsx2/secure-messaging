# Directories
SRC_DIR := src
INC_DIR := include
TEST_DIR := tests
APP_DIR := apps

BUILD_DIR := build
OBJ_DIR := obj
BIN_DIR := bin

# Files
## Source
SHARED_SRCS := $(shell find $(SRC_DIR) -name "*.cpp")
TEST_SRCS := $(shell find $(TEST_DIR) -name "*.cpp")
APP_SRCS := $(shell find $(APP_DIR) -name "*.cpp")
## Intermediary
OBJS := $(foreach obj, $(SHARED_SRCS:%.cpp=%.o), $(BUILD_DIR)/$(OBJ_DIR)/$(obj))
## Final
TESTS := $(foreach test, $(TEST_SRCS:%.cpp=%), $(BUILD_DIR)/$(BIN_DIR)/$(test))
APPS := $(foreach app, $(APP_SRCS:%.cpp=%), $(BUILD_DIR)/$(BIN_DIR)/$(app))

# Compiler settings
CC := g++
CFLAGS := -I$(INC_DIR) -I$(BUILD_DIR) -std=c++20 -Wall -Wextra -Werror
DEPS := -lcryptopp 

# Targets
all: tests apps
tests: $(TESTS)
apps: $(APPS)
clean:
	- rm -r $(BUILD_DIR)

$(BUILD_DIR)/$(BIN_DIR)/%: %.cpp $(OBJS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< $(OBJS) -o $@ $(DEPS)

$(BUILD_DIR)/$(OBJ_DIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@ $(DEPS)
