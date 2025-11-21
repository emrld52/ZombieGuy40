CC = gcc
LIBS += -lkernel32 -luser32 -lshell32 -lgdi32 -ld3d11 -ldxgi

BIN_DIR = bin
TARGET  = $(BIN_DIR)/zom.exe

SRC := $(wildcard src/*.c)

SHADER_FILE_INPUT  = shaders/vs_fs_shader.glsl
SHADER_FILE_OUTPUT = shaders/shaders.glsl.h

.PHONY: all rungame

all: $(TARGET) rungame

rungame:
	$(TARGET)

$(TARGET): $(SRC) $(SHADER_FILE_OUTPUT)
	$(CC) $(SRC) -o $(TARGET) $(LIBS) -static -lwinpthread

$(SHADER_FILE_OUTPUT): $(SHADER_FILE_INPUT)
	shaders/sokol-shdc --input $(SHADER_FILE_INPUT) --output $(SHADER_FILE_OUTPUT) --slang glsl430:hlsl5:metal_macos