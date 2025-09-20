# Compiler
CC = gcc

# Flags
CFLAGS = -Wall -Wextra -Iengine/include -Igame/include `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lSDL2_image -lm

# Source files
ENGINE_SRC = $(wildcard engine/src/*.c engine/tools/*.c)
GAME_SRC   = $(wildcard game/src/*.c)
SRC        = $(ENGINE_SRC) $(GAME_SRC)

# Target executable
TARGET = arpg

# Build target
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Clean build files
clean:
	rm -f $(TARGET)
