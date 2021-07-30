TARGET := dogesniffer

CPPFLAGS := -std=c++11
LDFLAGS := -lSDL2 -lSDL2_image

DEBUG ?= 0
ifeq ($(DEBUG),0)
	BUILD_DIR := ./build/release
	CPPFLAGS += -O2
else
	BUILD_DIR := ./build/debug
	CPPFLAGS += -O0 -g
endif

SRCS := main.cpp minefield.cpp minesweeper.cpp shuffle.cpp
OBJS := $(SRCS:%.c=$(BUILD_DIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
	rm $(TARGET)
