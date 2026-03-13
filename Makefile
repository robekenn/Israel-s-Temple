# Compiler definitions
CC_MAC = clang
CC_UNX = gcc
CC_WIN = gcc

# Targets
MAC_TARGET = Israels_Temple_mac
UNX_TARGET = Israels_Temple_linux
WIN_TARGET = Israels_Temple.exe

# App bundle
APP_NAME = Israels_Temple.app
APP_DIR = $(APP_NAME)/Contents
APP_MACOS_DIR = $(APP_DIR)/MacOS

# Main source folders to search
SRC_DIRS = Game Game/Character_System Game/MapLoader Game/cJSON Game/game Game/platformUtils Game/Inventory Game/Interactions/Bronze_Altar Game/Interactions/Incense_Altar

# Find all .c files in those folders
SRC = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))

# Raylib directories
RAYLIB_DIR = raylib
RAYLIB_SRC = $(RAYLIB_DIR)/src
RAYLIB_LIB = $(RAYLIB_SRC)/libraylib.a

# Include folders
INCLUDE_DIRS = -IGame/src -IGame/Character_System -I$(RAYLIB_SRC) -I$(RAYLIB_SRC)/external/glfw/include

# macOS frameworks needed for raylib
MAC_FRAMEWORKS = \
	-framework Cocoa \
	-framework OpenGL \
	-framework IOKit \
	-framework CoreVideo \
	-framework CoreAudio \
	-framework AudioToolbox \
	-framework AudioUnit

# -------------------------
# Raylib setup for Unix/macOS
# -------------------------
raylib_unx:
	@if [ ! -f "$(RAYLIB_LIB)" ]; then \
		if [ ! -d "$(RAYLIB_DIR)" ]; then \
			echo "Cloning raylib..."; \
			git clone --depth 1 https://github.com/raysan5/raylib.git; \
		fi; \
		echo "Building raylib..."; \
		cd $(RAYLIB_SRC) && $(MAKE) PLATFORM=PLATFORM_DESKTOP; \
	fi

# -------------------------
# Raylib setup for Windows
# -------------------------
raylib_win:
	@if [ ! -f "$(RAYLIB_LIB)" ]; then \
		if [ ! -d "$(RAYLIB_DIR)" ]; then \
			echo "Cloning raylib..."; \
			git clone --depth 1 https://github.com/raysan5/raylib.git; \
		fi; \
		echo "Building raylib for Windows..."; \
		cd $(RAYLIB_SRC) && mingw32-make PLATFORM=PLATFORM_DESKTOP; \
	fi

# -------------------------
# Mac build
# -------------------------
mac: raylib_unx
	$(CC_MAC) $(INCLUDE_DIRS) $(SRC) $(RAYLIB_LIB) $(MAC_FRAMEWORKS) -o $(MAC_TARGET)

mac_app: mac
	rm -rf $(APP_NAME)
	mkdir -p $(APP_MACOS_DIR)
	cp $(MAC_TARGET) $(APP_MACOS_DIR)/Israels_Temple
	cp -R Game $(APP_MACOS_DIR)/Game
	echo '<?xml version="1.0" encoding="UTF-8"?>' > $(APP_DIR)/Info.plist
	echo '<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">' >> $(APP_DIR)/Info.plist
	echo '<plist version="1.0">' >> $(APP_DIR)/Info.plist
	echo '<dict>' >> $(APP_DIR)/Info.plist
	echo '    <key>CFBundleExecutable</key>' >> $(APP_DIR)/Info.plist
	echo '    <string>Israels_Temple</string>' >> $(APP_DIR)/Info.plist
	echo '    <key>CFBundleIdentifier</key>' >> $(APP_DIR)/Info.plist
	echo '    <string>com.yourname.Israels_Temple</string>' >> $(APP_DIR)/Info.plist
	echo '    <key>CFBundleName</key>' >> $(APP_DIR)/Info.plist
	echo '    <string>Israels_Temple</string>' >> $(APP_DIR)/Info.plist
	echo '    <key>CFBundlePackageType</key>' >> $(APP_DIR)/Info.plist
	echo '    <string>APPL</string>' >> $(APP_DIR)/Info.plist
	echo '    <key>CFBundleVersion</key>' >> $(APP_DIR)/Info.plist
	echo '    <string>1.0.0</string>' >> $(APP_DIR)/Info.plist
	echo '    <key>CFBundleShortVersionString</key>' >> $(APP_DIR)/Info.plist
	echo '    <string>1.0.0</string>' >> $(APP_DIR)/Info.plist
	echo '</dict>' >> $(APP_DIR)/Info.plist
	echo '</plist>' >> $(APP_DIR)/Info.plist
	chmod +x $(APP_MACOS_DIR)/Israels_Temple

# -------------------------
# Linux build
# -------------------------
unx: raylib_unx
	$(CC_UNX) $(INCLUDE_DIRS) -o $(UNX_TARGET) $(SRC) $(RAYLIB_LIB) -lGL -lm -lpthread -ldl -lrt -lX11

# -------------------------
# Windows build
# -------------------------
win: raylib_win
	$(CC_WIN) $(INCLUDE_DIRS) -o $(WIN_TARGET) $(SRC) $(RAYLIB_LIB) -lopengl32 -lgdi32 -lwinmm

# -------------------------
# Build all
# -------------------------
all: mac_app unx win

# -------------------------
# Clean
# -------------------------
clean:
	rm -rf $(MAC_TARGET) $(UNX_TARGET) $(WIN_TARGET) $(APP_NAME)