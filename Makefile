# Compiler definitions
CC_MAC = clang
CC_UNX = gcc
CC_WIN = gcc

# Targets
MAC_TARGET = Tabernacle_mac
UNX_TARGET = Tabernacle_linux
WIN_TARGET = Tabernacle.exe

# App bundle
APP_NAME = Tabernacle.app
APP_DIR = $(APP_NAME)/Contents
APP_MACOS_DIR = $(APP_DIR)/MacOS

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
raylib_unx:
	if [ ! -f "$(RAYLIB_LIB)" ]; then \
		if [ ! -d "$(RAYLIB_DIR)" ]; then git clone --depth 1 https://github.com/raysan5/raylib.git; fi; \
		cd $(RAYLIB_SRC) && $(MAKE) PLATFORM=PLATFORM_DESKTOP; \
	fi

# -------------------------
# Raylib setup for Windows
# -------------------------
raylib_win:
	if [ ! -f "$(RAYLIB_LIB)" ]; then \
		if [ ! -d "$(RAYLIB_DIR)" ]; then git clone --depth 1 https://github.com/raysan5/raylib.git; fi; \
		cd $(RAYLIB_SRC) && mingw32-make PLATFORM=PLATFORM_DESKTOP; \
	fi

# -------------------------
# Mac build
# -------------------------
mac: raylib_unx
	$(CC_MAC) $(INCLUDE_DIRS) \
	-framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL \
	$(RAYLIB_LIB) $(SRC) -o $(MAC_TARGET)

mac_app: mac
	rm -rf $(APP_NAME)
	mkdir -p $(APP_MACOS_DIR)
	cp $(MAC_TARGET) $(APP_MACOS_DIR)/Tabernacle
	cp -R Game $(APP_MACOS_DIR)/Game
	echo '<?xml version="1.0" encoding="UTF-8"?>' > $(APP_DIR)/Info.plist
	echo '<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">' >> $(APP_DIR)/Info.plist
	echo '<plist version="1.0">' >> $(APP_DIR)/Info.plist
	echo '<dict>' >> $(APP_DIR)/Info.plist
	echo '    <key>CFBundleExecutable</key>' >> $(APP_DIR)/Info.plist
	echo '    <string>Tabernacle</string>' >> $(APP_DIR)/Info.plist
	echo '    <key>CFBundleIdentifier</key>' >> $(APP_DIR)/Info.plist
	echo '    <string>com.yourname.tabernacle</string>' >> $(APP_DIR)/Info.plist
	echo '    <key>CFBundleName</key>' >> $(APP_DIR)/Info.plist
	echo '    <string>Tabernacle</string>' >> $(APP_DIR)/Info.plist
	echo '    <key>CFBundlePackageType</key>' >> $(APP_DIR)/Info.plist
	echo '    <string>APPL</string>' >> $(APP_DIR)/Info.plist
	echo '    <key>CFBundleVersion</key>' >> $(APP_DIR)/Info.plist
	echo '    <string>1.0.0</string>' >> $(APP_DIR)/Info.plist
	echo '    <key>CFBundleShortVersionString</key>' >> $(APP_DIR)/Info.plist
	echo '    <string>1.0.0</string>' >> $(APP_DIR)/Info.plist
	echo '</dict>' >> $(APP_DIR)/Info.plist
	echo '</plist>' >> $(APP_DIR)/Info.plist
	chmod +x $(APP_MACOS_DIR)/Tabernacle

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
all: mac_app unx win

# -------------------------
# Clean
# -------------------------
clean:
	rm -rf $(MAC_TARGET) $(UNX_TARGET) $(WIN_TARGET) $(APP_NAME)