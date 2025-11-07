CC=gcc
LIBS+=-lkernel32 -luser32 -lshell32 -lgdi32 -ld3d11 -ldxgi

BIN_DIR = bin

TARGET = $(BIN_DIR)/zom.exe

rungame: $(TARGET)
	$(TARGET)

all: rungame

$(TARGET): src/main.c
	$(CC) src/main.c -o $(TARGET) $(LIBS)

all: $(TARGET)