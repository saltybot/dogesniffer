TARGET := dogesniffer

CFLAGS := -std=c99
LDFLAGS := -lSDL2 -lSDL2_image

DEBUG ?= 0
ifeq ($(DEBUG),0)
	BUILD_DIR := ./build/release
	CFLAGS += -O2
else
	BUILD_DIR := ./build/debug
	CFLAGS += -O0 -g
endif

SRCS := dogesniffer.c main.c mines.c util.c
OBJS := $(SRCS:%.c=$(BUILD_DIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
	rm $(TARGET)
