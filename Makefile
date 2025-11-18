CC=gcc
LIBS+=-lkernel32 -luser32 -lshell32 -lgdi32 -ld3d11 -ldxgi

BIN_DIR = bin

TARGET = $(BIN_DIR)/zom.exe

SRC += src/g_main.c src/r_renderfuncs.c src/z_coreloop.c src/z_player.c src/z_zombies.c src/s_tilemap.c src/s_scene.c src/s_entities.c src/s_animation.c src/s_weapons.c src/z_upgradecrate.c src/s_playerUI.c

SHADER_FILE_INPUT = shaders/vs_fs_shader.glsl
SHADER_FILE_OUTPUT = shaders/shaders.glsl.h

.PHONY: all $(SHADER_FILE_OUTPUT) $(TARGET)

all: $(TARGET) rungame

rungame:
	$(TARGET)

$(TARGET): $(SRC) $(SHADER_FILE_OUTPUT)
	$(CC) $(SRC) -o $(TARGET) $(LIBS) -static -lwinpthread -mwindows

$(SHADER_FILE_OUTPUT): $(SHADER_FILE_INPUT)
	shaders/sokol-shdc --input $(SHADER_FILE_INPUT) --output $(SHADER_FILE_OUTPUT) --slang glsl430:hlsl5:metal_macos