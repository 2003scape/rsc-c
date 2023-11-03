#CC = clang
DEBUG ?= 1
RENDER_GL ?= 0
SRC = $(wildcard src/*.c src/lib/*.c src/ui/*.c)
OBJ = $(SRC:.c=.o)
# remove -fwrapv when code is converted to use unsigned ints or
# overflow checks are added
CFLAGS += -fwrapv -DSDL12
# some platforms require gnu99 instead of c99 to use functions like alloca.
CFLAGS += -std=gnu99
CFLAGS += #-DREVISION_177
CFLAGS += $(shell sdl-config --cflags)
LDFLAGS += -lm
LDFLAGS += $(shell sdl-config --libs)

ifeq ($(RENDER_GL), 1)
SRC += $(wildcard src/gl/*.c src/gl/textures/*.c)
CFLAGS += -I ./cglm/include -DRENDER_GL #-DOPENGL20
CFLAGS += $(shell pkg-config --cflags SDL_image)
CFLAGS += $(shell pkg-config --cflags glew)
LDFLAGS += $(shell pkg-config --libs SDL_image)
LDFLAGS += $(shell pkg-config --libs glew)
else
CFLAGS += -DRENDER_SW
endif

ifeq ($(DEBUG), 1)
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