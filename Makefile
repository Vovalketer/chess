CC := gcc
CPPFLAGS := -MMD -MP
CFLAGS := -Wall -Wextra -std=c99 -pedantic -g
#we're using DNDEBUG on the tests to disable asserts, as they interfere with Criterion
CFLAGS_TEST := $(CFLAGS) -g
LIBS_SYSTEM := -lraylib
LDFLAGS := -lm $(LIBS_SYSTEM)

EXTERNAL = external
TESTING_LIB_DIR = $(EXTERNAL)/Criterion/lib
TESTING_LIB_LINK = -L$(TESTING_LIB_DIR) -Wl,-rpath=$(TESTING_LIB_DIR) -lcriterion
TESTING_INCLUDE = -I$(EXTERNAL)/Criterion/include

SRC_DIR := src
INC_DIR := include
BUILD_DIR := build
TARGET_BIN := chess

TEST_SRC := tests
TEST_DIR := ${BUILD_DIR}/tests

SRCS := $(shell find $(SRC_DIR) -name '*.c')
INCS := $(shell find $(INC_DIR) -type d)
INCS := $(addprefix -I,$(INCS))
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

DEPS := $(OBJS:%.o=%.d)
-include $(DEPS)

.PHONY: all
all: $(TARGET_BIN)

$(TARGET_BIN): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(INCS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR) $(TEST_SRC) $(TEST_DIR):
	mkdir -p $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

# Test sources and binaries (recursively find .c files)
TEST_SRCS := $(shell find tests -type f -name '*.c')
TEST_BINS := $(patsubst $(TEST_SRC)/%.c,$(TEST_DIR)/%,$(TEST_SRCS))
# filter out the main obj
MAIN_SRC := $(SRC_DIR)/main.c
MAIN_OBJ := $(BUILD_DIR)/$(subst $(SRC_DIR)/,,$(MAIN_SRC:.c=.o))
TEST_LINK_OBJS := $(filter-out $(MAIN_OBJ), $(OBJS))



$(TEST_DIR)/%: $(TEST_SRC)/%.c $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS_TEST) $(TESTING_INCLUDE) $(INCS) $< $(TEST_LINK_OBJS) -o $@ $(TESTING_LIB_LINK) $(LDFLAGS)


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
