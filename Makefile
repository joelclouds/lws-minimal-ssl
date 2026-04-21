CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -march=native -flto
LDFLAGS = -lwebsockets

ifdef DEBUG
    CFLAGS = -Wall -Wextra -std=c99 -g -DDEBUG -O0
    BUILD_DIR = build/debug
else
    CFLAGS += -DNDEBUG
    BUILD_DIR = build/release
endif

APP_SOURCES = main.c
APP_OBJECTS = $(APP_SOURCES:%.c=$(BUILD_DIR)/%.o)
TARGET = $(BUILD_DIR)/lws-test

all: $(TARGET)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@echo "[BUILD] Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(APP_OBJECTS)
	@echo "[LINK]  Linking $@"
	$(CC) $(APP_OBJECTS) -o $@ $(LDFLAGS)
	@echo "[DONE]  Build complete: $@"

deps:
	sudo apt-get update && sudo apt-get install -y build-essential libwebsockets-dev libssl-dev

clean:
	rm -rf build/

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean deps run
