# Compiler definitions
CC_MAC = clang
CC_UNX = gcc

# Targets
MAC_TARGET = Tabernacle_mac
UNX_TARGET = Tabernacle_unx

# Source file
SRC = Tabernacle.c

# Mac build
mac:
	$(CC_MAC) -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL Game/src/libraylib.a Game/$(SRC) -o $(MAC_TARGET)

# Linux / Windows (WSL or MinGW)
unx:
	$(CC_UNX) -o $(UNX_TARGET) Game/$(SRC) Game/src/libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11

# Build both
all: mac unx

# Clean build files
clean:
	rm -f $(MAC_TARGET) $(UNX_TARGET)