MAKEFLAGS += -j8

CC = gcc
LIBS += -lkernel32 -luser32 -lshell32 -lgdi32 -ld3d11 -ldxgi

BIN_DIR = bin
OBJ_DIR = obj
TARGET  = $(BIN_DIR)/zom.exe

SRC := $(wildcard src/*.c)
OBJ := $(SRC:src/%.c=$(OBJ_DIR)/%.o)

SHADER_FILE_INPUT  = shaders/vs_fs_shader.glsl
SHADER_FILE_OUTPUT = shaders/shaders.glsl.h

.PHONY: all rungame clean

all: $(TARGET)

rungame: $(TARGET)
	cd $(BIN_DIR) && ./zom.exe

$(TARGET): $(OBJ) | $(BIN_DIR)
	$(CC) $(OBJ) -o $(TARGET) $(LIBS) -static -lwinpthread

$(OBJ_DIR)/%.o: src/%.c $(SHADER_FILE_OUTPUT) | $(OBJ_DIR)
	$(CC) -c $< -o $@

$(SHADER_FILE_OUTPUT): $(SHADER_FILE_INPUT)
	shaders/sokol-shdc --input $(SHADER_FILE_INPUT) --output $(SHADER_FILE_OUTPUT) --slang glsl430:hlsl5:metal_macos

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	if exist $(OBJ_DIR) rmdir /s /q $(OBJ_DIR)