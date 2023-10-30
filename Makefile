#CC = clang
DEBUG = 1
#RENDER_GL = 1 TODO
#SRC = $(wildcard src/*.c src/lib/*.c src/ui/*.c)
SRC = $(wildcard src/*.c src/lib/*.c src/ui/*.c src/gl/*.c src/gl/textures/*.c)
OBJ = $(SRC:.c=.o)
# remove -fwrapv when code is converted to use unsigned ints or
# overflow checks are added
CFLAGS += -fwrapv
# some platforms require gnu99 instead of c99 to use functions like alloca.
CFLAGS += -std=gnu99
CFLAGS += -I ./cglm/include -DRENDER_GL #-DRENDER_SW #-DREVISION_177 #-DOPENGL20 #-DSDL12
CFLAGS += $(shell sdl2-config --cflags)
CFLAGS += $(shell pkg-config --cflags SDL2_image)
CFLAGS += $(shell pkg-config --cflags glew)
LDFLAGS += -lm
LDFLAGS += $(shell sdl2-config --libs)
LDFLAGS += $(shell pkg-config --libs SDL2_image)
LDFLAGS += $(shell pkg-config --libs glew)

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
