TARGET ?= bin/RainCife

CC = gcc
PKG_CONFIG ?= pkg-config

SRC_DIR = src
OBJ_DIR = objects

INCLUDES = -I./include

ifneq ($(wildcard external/raydial/include),)
INCLUDES += -I./external/raydial/include
endif

RAYLIB_CFLAGS = $(shell $(PKG_CONFIG) --cflags raylib 2>/dev/null)
RAYLIB_LIBS = $(shell $(PKG_CONFIG) --libs raylib 2>/dev/null)

ifeq ($(strip $(RAYLIB_LIBS)),)
RAYLIB_LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

CFLAGS ?= -std=c99 -Wall -Wextra -g -O0 -DDEBUG -DPLATFORM_DESKTOP
CPPFLAGS ?= $(INCLUDES) $(RAYLIB_CFLAGS)
LDLIBS ?= $(RAYLIB_LIBS) -lm

SRC = $(wildcard $(SRC_DIR)/*.c) \
      $(wildcard $(SRC_DIR)/*/*.c)

RAYDIAL_SRC = $(wildcard external/raydial/src/*.c)

OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))
RAYDIAL_OBJ = $(patsubst external/raydial/src/%.c,$(OBJ_DIR)/raydial/%.o,$(RAYDIAL_SRC))

ALL_OBJ = $(OBJ) $(RAYDIAL_OBJ)

all: dirs $(TARGET)

$(TARGET): $(ALL_OBJ) | dirs
	$(CC) $(ALL_OBJ) -o $(TARGET) $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/raydial/%.o: external/raydial/src/%.c
	mkdir -p $(dir $@)
	$(CC) -w $(CPPFLAGS) $(CFLAGS) -c $< -o $@

dirs:
	mkdir -p bin
	mkdir -p objects
	mkdir -p objects/raydial

run: all
	./$(TARGET)

clean:
	rm -rf bin objects

info:
	@echo SRC: $(SRC)
	@echo RAYDIAL_SRC: $(RAYDIAL_SRC)
	@echo OBJ: $(OBJ)
	@echo ALL_OBJ: $(ALL_OBJ)

.PHONY: all run clean dirs info
