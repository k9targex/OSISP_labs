CC = gcc
CFLAGS_COMMON = -std=c11 -pedantic -W -Wall -Wno-unused-parameter -Wno-unused-variable
CFLAGS_DEBUG = $(CFLAGS_COMMON) -g -ggdb
CFLAGS_RELEASE = $(CFLAGS_COMMON) -Werror

SRCS = src/main.c
OBJS = $(SRCS:src/%.c=$(BIN_DIR)/%.o)
EXEC = laba5
BIN_DIR = bin

.PHONY: all clean

all: $(BIN_DIR)/$(EXEC)

$(BIN_DIR)/$(EXEC): $(OBJS) | $(BIN_DIR)
ifeq ($(MODE),debug)
	$(CC) $(CFLAGS_DEBUG) $(OBJS) -o $@
else
	$(CC) $(CFLAGS_RELEASE) $(OBJS) -o $@
endif

$(BIN_DIR)/%.o: src/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS_COMMON) -c $< -o $@

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BIN_DIR)
