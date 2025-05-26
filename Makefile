CC := gcc
CPPFLAGS := -MMD -MP
CFLAGS := -Wall -Wextra -std=c99 -pedantic
#we're using DNDEBUG on the tests to disable asserts, as they interfere with Criterion
CFLAGS_TEST := $(CFLAGS) -g -DNDEBUG
LIBS_SYSTEM := -lraylib
LDFLAGS := -lm $(LIBS_SYSTEM)

EXTERNAL = external
TESTING_LIB_DIR = $(EXTERNAL)/Criterion/lib
TESTING_LIB_LINK = -L$(TESTING_LIB_DIR) -Wl,-rpath=$(TESTING_LIB_DIR) -lcriterion
TESTING_INCLUDE = $(EXTERNAL)/Criterion/include

SRC_DIR := src
BUILD_DIR := build
OBJ_DIR := ${BUILD_DIR}/obj
TARGET_BIN := chess

TEST_SRC := tests
TEST_OBJ_DIR := ${BUILD_DIR}/tests/obj
TEST_DIR := ${BUILD_DIR}/tests

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TEST_OBJS := $(SRCS:$(SRC_DIR)/%.c=$(TEST_OBJ_DIR)/%.o)

DEPS := $(OBJS:%.o=%.d)
-include $(DEPS)


all: $(TARGET_BIN)

$(TARGET_BIN): $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR) $(OBJ_DIR) $(DEP_DIR) $(TEST_SRC) $(TEST_DIR) $(TEST_OBJ_DIR):
	mkdir -p $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

# Test sources and binaries (recursively find .c files)
TEST_SRCS := $(shell find tests -type f -name '*.c')
TEST_BINS := $(patsubst tests/%.c,$(TEST_DIR)/%,$(TEST_SRCS))
# filter out the main obj
MAIN_SRC := $(SRC_DIR)/main.c
MAIN_OBJ := $(TEST_OBJ_DIR)/$(subst $(SRC_DIR)/,,$(MAIN_SRC:.c=.o))
TEST_LINK_OBJS := $(filter-out $(MAIN_OBJ), $(TEST_OBJS))
# Compile each test binary, preserving subdirectory structure
$(TEST_DIR)/%: tests/%.c $(TEST_OBJS) | $(TEST_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS_TEST) -I$(TESTING_INCLUDE) $< $(TEST_LINK_OBJS) -o $@ $(TESTING_LIB_LINK) $(LDFLAGS) 

$(TEST_OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(TEST_OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS_TEST) -c $< -o $@

# Run all tests
.PHONY: test
test: $(TEST_BINS)
	@echo "Running all tests:"
	@echo "";
	@for t in $^; do \
		echo "=== $$t ==="; \
		./$$t; \
		echo ""; \
	done
