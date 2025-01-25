CC = gcc
CFLAGS = $(shell pkg-config --cflags sdl2 SDL2_mixer gl)
LDFLAGS = $(shell pkg-config --libs sdl2 SDL2_mixer gl) -lm
TARGET = musical_circle

all: $(TARGET)

$(TARGET): main.c
	$(CC) main.c -o $(TARGET) $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET)
