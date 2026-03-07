# Compiler definitions
CC_MAC = clang
CC_UNX = gcc

# Targets
MAC_TARGET = Tabernacle_mac
UNX_TARGET = Tabernacle_linux

# Main source folders to search
SRC_DIRS = Game Game/Character_System

# Find all .c files in those folders
SRC = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))

# Optional include folders
INCLUDE_DIRS = -IGame/src -IGame/Character_System

# Mac build
mac:
	$(CC_MAC) $(INCLUDE_DIRS) \
	-framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL \
	Game/src/libraylib.a $(SRC) -o $(MAC_TARGET)

# Linux / Windows (WSL or MinGW)
unx:
	$(CC_UNX) $(INCLUDE_DIRS) \
	-o $(UNX_TARGET) $(SRC) \
	-lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Build both
all: mac unx

# Clean build files
clean:
	rm -f $(MAC_TARGET) $(UNX_TARGET)