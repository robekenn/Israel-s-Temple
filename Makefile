# Compiler definitions
CC_MAC = clang
CC_UNX = gcc
CC_WIN = gcc

# Targets
MAC_TARGET = Tabernacle_mac
UNX_TARGET = Tabernacle_unx
WIN_TARGET = Tabernacle.exe

# Main source folders to search
SRC_DIRS = Game Game/Character_System

# Find all .c files in those folders
SRC = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))

# Include folders
INCLUDE_DIRS = -IGame/src -IGame/Character_System

# Mac build
mac:
	$(CC_MAC) $(INCLUDE_DIRS) \
	-framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL \
	Game/src/libraylib.a $(SRC) -o $(MAC_TARGET)

# Linux build
unx:
	$(CC_UNX) $(INCLUDE_DIRS) \
	-o $(UNX_TARGET) $(SRC) \
	-lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Windows build
win:
	$(CC_WIN) $(INCLUDE_DIRS) \
	-o $(WIN_TARGET) $(SRC) \
	-lraylib -lopengl32 -lgdi32 -lwinmm

# Build all
all: mac unx win

# Clean build files
clean:
	rm -f $(MAC_TARGET) $(UNX_TARGET) $(WIN_TARGET)