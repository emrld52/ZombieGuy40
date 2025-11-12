NUMBER_OF_PROCESSORS = 8

MAKEFLAGS += -j$(NUMBER_OF_PROCESSORS)

CC=gcc
LIBS+=-lkernel32 -luser32 -lshell32 -lgdi32 -ld3d11 -ldxgi
CFLAGS = -O2

BIN_DIR = bin

TARGET = $(BIN_DIR)/zom.exe

SRC += src/g_main.c src/r_renderfuncs.c src/z_coreloop.c src/z_player.c src/z_zombies.c

SHADER_FILE_INPUT = shaders/vs_fs_shader.glsl
SHADER_FILE_OUTPUT = shaders/shaders.glsl.h

all: $(TARGET) rungame

rungame:
	$(TARGET)

$(TARGET): $(SRC)
	sokol-shdc --input $(SHADER_FILE_INPUT) --output $(SHADER_FILE_OUTPUT) --slang glsl430:hlsl5:metal_macos
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

shader:
	sokol-shdc --input $(SHADER_FILE_INPUT) --output $(SHADER_FILE_OUTPUT) --slang glsl430:hlsl5:metal_macos