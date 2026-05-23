ifeq ($(OS),Windows_NT)
    PLATFORM_OS = WINDOWS
    EXT        = .exe

    ifneq ($(wildcard C:/raylib/w64devkit/bin/gcc.exe),)
        CC       = C:/raylib/w64devkit/bin/gcc.exe
        INCLUDES = -I./include -IC:/raylib-5.5_win64_mingw-w64/include -IC:/raylib/raylib/src
        LIBS     = -LC:/raylib-5.5_win64_mingw-w64/lib -lraylib -lopengl32 -lgdi32 -lwinmm
    else
        CC       = C:/msys64/mingw64/bin/gcc.exe
        INCLUDES = -I./include -IC:/msys64/mingw64/include
        LIBS     = -LC:/msys64/mingw64/lib -lraylib -lopengl32 -lgdi32 -lwinmm
    endif

    RM       = rmdir /s /q
    FIX_PATH = $(subst /,\,$1)
else
    PLATFORM_OS = LINUX
    EXT        =
    CC         = gcc
    INCLUDES   = -I./include
    LIBS       = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -lXrandr -lXinerama -lXi -lXcursor
    RM         = rm -rf
    FIX_PATH   = $1
endif

CFLAGS  = -std=c99 -Wall -Wextra -DPLATFORM_DESKTOP
LDFLAGS =

ifeq ($(BUILD),release)
    CFLAGS += -O2 -DNDEBUG
else
    CFLAGS += -g -O0 -DDEBUG
endif

TARGET = bin/RatTsunami$(EXT)

SRC = $(wildcard src/*.c) \
      $(wildcard src/core/*.c) \
      $(wildcard src/entities/*.c) \
      $(wildcard src/graphics/*.c) \
      $(wildcard src/gameplay/*.c)

OBJ = $(patsubst src/%.c,obj/%.o,$(SRC))

.PHONY: all run clean dirs release info

all: dirs $(TARGET)

release:
	$(MAKE) BUILD=release

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LIBS) $(LDFLAGS)

obj/%.o: src/%.c
ifeq ($(OS),Windows_NT)
	@if not exist "$(call FIX_PATH,$(dir $@))" mkdir "$(call FIX_PATH,$(dir $@))"
else
	@mkdir -p "$(dir $@)"
endif
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

run: all
ifeq ($(OS),Windows_NT)
	$(TARGET)
else
	@if [ -f .env ]; then set -a; . ./.env; set +a; fi; ./$(TARGET)
endif

clean:
ifeq ($(OS),Windows_NT)
	@if exist bin $(RM) bin
	@if exist obj $(RM) obj
else
	$(RM) bin obj
endif

dirs:
ifeq ($(OS),Windows_NT)
	@if not exist bin mkdir bin
	@if not exist obj mkdir obj
else
	@mkdir -p bin obj
endif

info:
	@echo Platform : $(PLATFORM_OS)
	@echo Compiler : $(CC)
	@echo Target   : $(TARGET)
	@echo Sources  : $(SRC)
	@echo Objects  : $(OBJ)
	@echo Includes : $(INCLUDES)
	@echo Libs     : $(LIBS)
	@echo Build    : $(if $(filter release,$(BUILD)),release,debug)