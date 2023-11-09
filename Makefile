#CC = clang
SRC = $(wildcard src/*.c src/lib/*.c src/ui/*.c src/custom/*.c)
OBJ = $(SRC:.c=.o)
# remove -fwrapv when code is converted to use unsigned ints or
# overflow checks are added
CFLAGS += -fwrapv
# some platforms require gnu99 instead of c99 to use functions like alloca.
CFLAGS += -std=gnu99
CFLAGS += #-DREVISION_177
CFLAGS += $(shell sdl2-config --cflags)
LDFLAGS += -lm
LDFLAGS += $(shell sdl2-config --libs)

ifeq ($(RENDER_GL), 1)
SRC += $(wildcard src/gl/*.c src/gl/textures/*.c)
CFLAGS += -I ./cglm/include -DRENDER_GL
CFLAGS += $(shell pkg-config --cflags SDL2_image)
CFLAGS += $(shell pkg-config --cflags glew)
LDFLAGS += $(shell pkg-config --libs SDL2_image)
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
