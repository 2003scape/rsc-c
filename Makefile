#CC = clang
DEBUG = 1
#RENDER_GL = 1 TODO
#SRC = $(wildcard src/*.c src/lib/*.c src/ui/*.c)
SRC = $(wildcard src/*.c src/lib/*.c src/ui/*.c src/gl/*.c src/gl/textures/*.c)
OBJ = $(SRC:.c=.o)
CFLAGS = -I ./cglm/include -DRENDER_GL #-DRENDER_SW #-DREVISION_177
LDFLAGS = -lm -lSDL2 -lSDL2_image -lGLEW -lGL

ifdef DEBUG
CFLAGS += -Wall -Wextra -pedantic -g
#LDFLAGS += -fsanitize=address -static-libasan
else
CFLAGS += -s -Ofast
LDFLAGS += -s
endif

mudclient: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f src/*.o src/lib/*.o src/ui/*.o src/gl/*.o src/gl/textures/*.o
	rm -f mudclient
