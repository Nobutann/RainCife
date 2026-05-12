ifeq ($(OS),Windows_NT)
    PLATFORM_OS = WINDOWS
    EXT        = .exe
    ifneq ($(wildcard C:/msys64/mingw64/bin/gcc.exe),)
        CC       = C:/msys64/mingw64/bin/gcc.exe
        INCLUDES = -I./include -IC:/msys64/mingw64/include
        LIBS     = -LC:/msys64/mingw64/lib -lraylib -lopengl32 -lgdi32 -lwinmm
    else
        CC       = C:/raylib/w64devkit/bin/gcc.exe
        INCLUDES = -I./include -IC:/raylib-5.5_win64_mingw-w64/include -IC:/raylib/raylib/src
        LIBS     = -LC:/raylib-5.5_win64_mingw-w64/lib -lraylib -lopengl32 -lgdi32 -lwinmm
    endif
    RM       = rd /s /q
    MKDIR    = mkdir
    FIX_PATH = $(subst /,\,$1)
else
    PLATFORM_OS = LINUX
    EXT        =
    CC         = gcc
    INCLUDES   = -I./include
    LIBS       = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -lXrandr -lXinerama -lXi -lXcursor
    RM         = rm -rf
    MKDIR      = mkdir -p
    FIX_PATH   = $1
endif

CFLAGS  = -std=c99 -Wall -Wextra -DPLATFORM_DESKTOP
LDFLAGS =

# Build mode: make BUILD=release
ifeq ($(BUILD),release)
    CFLAGS += -O2 -DNDEBUG
else
    CFLAGS += -g -O0 -DDEBUG
endif

TARGET = bin/RatTsunami$(EXT)
SRC    = $(wildcard src/*.c) $(wildcard src/**/*.c)
OBJ    = $(patsubst src/%.c, obj/%.o, $(SRC))

# ── Targets ────────────────────────────────────────────────────────────────────

.PHONY: all run clean dirs release info

all: dirs $(TARGET)

release:
	$(MAKE) BUILD=release

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LIBS) $(LDFLAGS)

obj/%.o: src/%.c
	-$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

run: all
	./$(TARGET)

clean:
	$(RM) $(call FIX_PATH,bin) $(call FIX_PATH,obj)



dirs:
	-$(MKDIR) bin
	-$(MKDIR) obj

info:
	@echo "Platform : $(PLATFORM_OS)"
	@echo "Compiler : $(CC)"
	@echo "Target   : $(TARGET)"
	@echo "Sources  : $(SRC)"
	@echo "Build    : $(if $(filter release,$(BUILD)),release,debug)"
