CC_MAC = clang
CC_UNX = gcc

MAC_TARGET = Tabernacle_mac
UNX_TARGET = Tabernacle_unx
SRC = Tabernacle.c

mac:
	$(CC_MAC) -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL libraylib.a $(SRC) -o $(MAC_TARGET)

unx:
	$(CC_UNX) -o $(UNX_TARGET) $(SRC) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

all: mac unx

clean:
	rm -f $(MAC_TARGET) $(UNX_TARGET)