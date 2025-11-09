CC=gcc
LIBS+=-lkernel32 -luser32 -lshell32 -lgdi32 -ld3d11 -ldxgi

BIN_DIR = bin

TARGET = $(BIN_DIR)/zom.exe

all: $(TARGET) rungame

rungame:
	$(TARGET)

$(TARGET): src/g_main.c src/r_renderfuncs.c src/z_coreloop.c shaders/vs_fs_shader.glsl
	sokol-shdc --input shaders/vs_fs_shader.glsl --output shaders/shaders.glsl.h --slang glsl430:hlsl5:metal_macos
	$(CC) src/g_main.c src/r_renderfuncs.c src/z_coreloop.c -o $(TARGET) $(LIBS)

shader:
	sokol-shdc --input shaders/vs_fs_shader.glsl --output shaders/shaders.glsl.h --slang glsl430:hlsl5:metal_macos