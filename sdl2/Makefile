TARGET := dogesniffer

BUILD_DIR = ./build

SRCS := main.c
OBJS := $(SRCS:%.c=$(BUILD_DIR)/%.o)

CFLAGS := -std=c99
LDFLAGS := -lSDL2 -lSDL2_image

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
	rm $(TARGET)
