# Directories
BUILD_DIR := build
SRC_DIR := src
INC_DIR := include
TEST_DIR := tests

# Files
SRCS := $(shell find $(SRC_DIR) -name "*.cpp")
OBJS := $(foreach obj, $(SRCS:%.cpp=%.o), $(BUILD_DIR)/$(obj))
TESTS := $(shell find $(TEST_DIR) -name "*.cpp")
EXES := $(foreach exe, $(TESTS:%.cpp=%.exe), $(BUILD_DIR)/$(notdir $(exe)))

# Compiler settings
CC := g++
CFLAGS := -I$(INC_DIR) -I$(BUILD_DIR)
DEPS := -lcryptopp 

# Targets
all: $(EXES)
clean:
	rm -r $(BUILD_DIR)

$(EXES): $(BUILD_DIR)/%.exe: $(TEST_DIR)/%.cpp $(OBJS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< $(OBJS) -o $@ $(DEPS)

$(OBJS): $(BUILD_DIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@ $(DEPS)
