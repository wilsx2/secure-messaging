# Output
EXE := build/main.exe

# Directories
BUILD_DIR := build
SRC_DIR := src
INC_DIR := include

# Files
SRCS := $(shell find $(SRC_DIR) -name "*.cpp")
OBJS := $(foreach obj, $(SRCS:%.cpp=%.o), $(BUILD_DIR)/$(obj))

# Compiler settings
CC := g++
CFLAGS := -I$(INC_DIR) -I$(BUILD_DIR)
DEPS := -lcryptopp 

# Targets
all: $(EXE)
clean:
	rm $(OBJS) $(EXE)

$(EXE): $(BUILD_DIR)/%.exe: $(OBJS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(DEPS)

$(OBJS): $(BUILD_DIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@ $(DEPS)
