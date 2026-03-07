# Compiler definitions
CC_MAC = clang
CC_UNX = gcc
CC_WIN = gcc

# Targets
MAC_TARGET = Tabernacle_mac
UNX_TARGET = Tabernacle_linux
WIN_TARGET = Tabernacle.exe

# Main source folders to search
SRC_DIRS = Game Game/Character_System

# Find all .c files in those folders
SRC = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))

# Raylib directories
RAYLIB_DIR = raylib
RAYLIB_SRC = $(RAYLIB_DIR)/src
RAYLIB_LIB = $(RAYLIB_SRC)/libraylib.a

# Include folders
INCLUDE_DIRS = -IGame/src -IGame/Character_System -I$(RAYLIB_SRC) -I$(RAYLIB_SRC)/external/glfw/include

# -------------------------
# Raylib setup for Unix/macOS
# -------------------------
# Raylib setup for Unix/macOS
raylib_unx:
	if [ ! -d "$(RAYLIB_DIR)" ]; then git clone --depth 1 https://github.com/raysan5/raylib.git; fi
	cd $(RAYLIB_SRC) && $(MAKE) PLATFORM=PLATFORM_DESKTOP

# Raylib setup for Windows
raylib_win:
	if [ ! -d "$(RAYLIB_DIR)" ]; then git clone --depth 1 https://github.com/raysan5/raylib.git; fi
	cd $(RAYLIB_SRC) && mingw32-make PLATFORM=PLATFORM_DESKTOP
	

# -------------------------
# Mac build
# -------------------------
mac: raylib_unx
	$(CC_MAC) $(INCLUDE_DIRS) \
	-framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL \
	$(RAYLIB_LIB) $(SRC) -o $(MAC_TARGET)

# -------------------------
# Linux build
# -------------------------
unx: raylib_unx
	$(CC_UNX) $(INCLUDE_DIRS) \
	-o $(UNX_TARGET) $(SRC) \
	$(RAYLIB_LIB) -lGL -lm -lpthread -ldl -lrt -lX11

# -------------------------
# Windows build
# -------------------------
win: raylib_win
	$(CC_WIN) $(INCLUDE_DIRS) \
	-o $(WIN_TARGET) $(SRC) \
	$(RAYLIB_LIB) -lopengl32 -lgdi32 -lwinmm

# -------------------------
# Build all
# -------------------------
all: mac unx win

# -------------------------
# Clean
# -------------------------
clean:
	rm -f $(MAC_TARGET) $(UNX_TARGET) $(WIN_TARGET)