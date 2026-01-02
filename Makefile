CXX = clang++
CXX_FLAGS = -Wall -Wextra -std=c++23 -I./lib/include
DEBUG_FLAGS = -g -O0 -fsanitize=address

# Directories
DIR_LIB_SRC = lib/src
DIR_LIB_INC = lib/include

DIR_BUILD = build
DIR_BUILD_OBJ = $(DIR_BUILD)/obj
DIR_DEBUG_OBJ = $(DIR_BUILD)/debug_obj

DIR_TEST = lib/tests
DIR_TEST_BIN = $(DIR_BUILD)/bin

# Library
LIB_NAME = libdove
LIB_PATH = $(DIR_BUILD)/$(LIB_NAME).a
LIB_DEBUG_PATH = $(DIR_BUILD)/$(LIB_NAME)_debug.a

LIB_SRC = $(shell find $(DIR_LIB_SRC) -name '*.cpp')
LIB_OBJ = $(patsubst $(DIR_LIB_SRC)/%.cpp,$(DIR_BUILD_OBJ)/%.o,$(LIB_SRC))
LIB_DEBUG_OBJ = $(patsubst $(DIR_LIB_SRC)/%.cpp,$(DIR_DEBUG_OBJ)/%.o,$(LIB_SRC))

# Tests
TEST_SRC = $(wildcard $(DIR_TEST)/*.cpp)
TEST_BIN = $(patsubst $(DIR_TEST)/%.cpp,$(DIR_TEST_BIN)/%,$(TEST_SRC))

# Default (All)
.PHONY: all
all: debug clangd tests

# Build (Release)
.PHONY: release
release: $(LIB_PATH)
	@echo "Release build complete"

# Build (Debug)
.PHONY: debug
debug: $(LIB_DEBUG_PATH)
	@echo "Debug build complete"

# Build (Tests)
.PHONY: tests
tests: $(TEST_BIN)
	@echo "Tests built"

# Create static library
$(LIB_PATH): $(LIB_OBJ)
	@mkdir -p $(DIR_BUILD)
	ar rcs $@ $^
	@echo "Built release library: $(LIB_PATH)"

$(LIB_DEBUG_PATH): $(LIB_DEBUG_OBJ)
	@mkdir -p $(DIR_BUILD)
	ar rcs $@ $^
	@echo "Built debug library: $(LIB_DEBUG_PATH)"

# Compile source files to object files
$(DIR_BUILD_OBJ)/%.o: $(DIR_LIB_SRC)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) -c $< -o $@

$(DIR_DEBUG_OBJ)/%.o: $(DIR_LIB_SRC)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) $(DEBUG_FLAGS) -c $< -o $@

# Compile individual test binaries
$(DIR_TEST_BIN)/%: $(DIR_TEST)/%.cpp $(LIB_DEBUG_PATH)
	@mkdir -p $(DIR_TEST_BIN)
	$(CXX) $(CXX_FLAGS) $(DEBUG_FLAGS) $< -L$(DIR_BUILD) -ldove_debug -o $@
	@echo "Built test: $@"

# Generate clangd configurations
.PHONY: clangd
clangd:
	@echo "Generating compile_commands.json..."
	@echo "[" > compile_commands.json; \
	first=true; \
	for src in $(LIB_SRC); do \
		if [ "$$first" = true ]; then \
			first=false; \
		else \
			echo "," >> compile_commands.json; \
		fi; \
		echo "  {" >> compile_commands.json; \
		echo "    \"directory\": \"$$(pwd)\"," >> compile_commands.json; \
		echo "    \"command\": \"$(CXX) $(CXX_FLAGS) -c $$src\"," >> compile_commands.json; \
		echo "    \"file\": \"$$src\"" >> compile_commands.json; \
		printf "  }" >> compile_commands.json; \
	done; \
	echo "" >> compile_commands.json; \
	echo "]" >> compile_commands.json
	@echo "compile_commands.json generated"

# Clean all artifacts
.PHONY: clean
clean:
	rm -rf $(DIR_BUILD)
	rm -rf .cache
	rm -f compile_commands.json
	rm -f .clangd
	@echo "Clean complete"

# Help
.PHONY: help
help:
	@echo "Available targets:"
	@echo "  make release               - Build the release libdove.a static library"
	@echo "  make debug                 - Build the debug libdove_debug.a static library"
	@echo "  make tests                 - Build debug library and compile all tests"
	@echo "  make clangd                - Generate clangd configurations"
	@echo "  make clean                 - Clean files & directories"
	@echo "  make help                  - Display this help message"
