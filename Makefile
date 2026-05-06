# Detecta automaticamente qual toolchain esta instalada
ifneq ($(wildcard C:/msys64/mingw64/bin/gcc.exe),)
    CC       = C:/msys64/mingw64/bin/gcc.exe
    INCLUDES = -I./include -IC:/msys64/mingw64/include
    LIBS     = -LC:/msys64/mingw64/lib -lraylib -lopengl32 -lgdi32 -lwinmm
else
    CC       = C:/raylib/w64devkit/bin/gcc.exe
    INCLUDES = -I./include -IC:/raylib-5.5_win64_mingw-w64/include
    LIBS     = -LC:/raylib-5.5_win64_mingw-w64/lib -lraylib -lopengl32 -lgdi32 -lwinmm
endif

CFLAGS = -std=c99 -Wall -Wextra -DPLATFORM_DESKTOP

TARGET = bin/RatTsunami.exe
SRC = $(wildcard src/*.c) $(wildcard src/**/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

all: dirs $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $^ -o $@ $(LIBS)

obj/%.o: src/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

run: all
	./$(TARGET)

clean:
	-rm -rf bin/ obj/ 2>nul
	-rd /s /q bin 2>nul
	-rd /s /q obj 2>nul

dirs:
	-mkdir bin 2>nul
	-mkdir obj 2>nul
	-mkdir obj\core 2>nul
	-mkdir obj\entities 2>nul
	-mkdir obj\graphics 2>nul
	-mkdir obj\gameplay 2>nul

.PHONY: all run clean dirs
