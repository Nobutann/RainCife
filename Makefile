TARGET = bin/RainCife.exe

CC = C:/raylib/w64devkit/bin/gcc.exe

SRC_DIR = src
OBJ_DIR = objects

INCLUDES = -I./include \
           -I./external/raydial/include \
           -IC:/raylib-5.5_win64_mingw-w64/include \
           -IC:/raylib/raylib/src

CFLAGS = -std=c99 -Wall -Wextra -g -O0 -DDEBUG -DPLATFORM_DESKTOP $(INCLUDES)

LIBS = -LC:/raylib-5.5_win64_mingw-w64/lib \
       -lraylib -lopengl32 -lgdi32 -lwinmm

SRC = $(wildcard $(SRC_DIR)/*.c) \
      $(wildcard $(SRC_DIR)/*/*.c)

RAYDIAL_SRC = $(wildcard external/raydial/src/*.c)

OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))
RAYDIAL_OBJ = $(patsubst external/raydial/src/%.c,$(OBJ_DIR)/raydial/%.o,$(RAYDIAL_SRC))

ALL_OBJ = $(OBJ) $(RAYDIAL_OBJ)

all: dirs $(TARGET)

$(TARGET): $(ALL_OBJ)
	$(CC) $(ALL_OBJ) -o $(TARGET) $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/raydial/%.o: external/raydial/src/%.c
	mkdir -p $(dir $@)
	$(CC) -w $(CFLAGS) -c $< -o $@

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