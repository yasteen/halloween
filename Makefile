CC=g++
EXTENSION = cpp

CFLAGS=-std=c++20 -Wall -Wextra -Wpedantic
LDFLAGS=$(shell pkg-config --libs ncurses)

SOURCE_DIR=src
BUILD_DIR=objs

TARGET ?= exe
SOURCES := $(shell find $(SOURCE_DIR) -name '*.$(EXTENSION)')


# --- FUNCTIONS --- #

# Gets corresponding object and dependency files from source files
define get_obj
	$(patsubst %.$(EXTENSION),$(BUILD_DIR)/%.o,$(notdir $(1)))
endef
define get_dep
	$(patsubst %.$(EXTENSION),$(BUILD_DIR)/%.d,$(notdir $(1)))
endef

# Creates an obj file from a given C file
define mk_c_rule
$(patsubst %.$(EXTENSION),$(BUILD_DIR)/%.o,$(notdir $(1))): $(1) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $$< -o $$@
endef


# --- HELPER VARIBLES --- #

# List of all relevant object files
OBJ_FILES := $(call get_obj,$(SOURCES))
DEP_FILES := $(call get_dep,$(SOURCES))


# --- BUILD RULES --- #

# Build all
all: $(TARGET)

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $@

# Build object files
-include $(DEP_FILES)
$(foreach f, $(SOURCES), $(eval $(call mk_c_rule, $(f))))

# Build executable
$(TARGET): $(OBJ_FILES)
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@

clean:
	rm -f $(TARGET)
	rm -rf $(BUILD_DIR)

run:
	./$(TARGET)

.PHONY: clean run all
