CC=gcc
LIBS+=-lkernel32 -luser32 -lshell32 -lgdi32 -ld3d11 -ldxgi

BIN_DIR = bin

TARGET = $(BIN_DIR)/zom.exe

all: $(TARGET) rungame

rungame:
	$(TARGET)

$(TARGET): src/main.c
	$(CC) src/main.c -o $(TARGET) $(LIBS)