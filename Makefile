#CC = clang
DEBUG ?= 1
SDL2 ?= 1
GLAD ?= 0
RENDER_GL ?= 0
LEGACY_GL ?= 0
VANILLA_IS_DEFAULT ?= 0
USE_LOCOLOUR ?= 0
USE_TOONSCAPE ?= 0
WITH_OPENSSL ?= 0

DESTDIR?=
PREFIX?= /usr/local
BINDIR?= bin
DATAROOTDIR?= share
DATADIR?= $(DATAROOTDIR)/rsc-c

SRC = $(wildcard src/*.c src/lib/*.c src/lib/rsa/*.c src/ui/*.c src/custom/*.c)
OBJ = $(SRC:.c=.o)

# remove -fwrapv when code is converted to use unsigned ints or
# overflow checks are added
CFLAGS += -fwrapv
# some platforms require gnu99 instead of c99 to use functions like alloca.
CFLAGS += -std=gnu99
CFLAGS += -DMUD_DATADIR=\"$(PREFIX)/$(DATADIR)\"

ifeq ($(SDL2), 1)
CFLAGS += $(shell sdl2-config --cflags) -DSDL2
LDFLAGS += -lm
LDFLAGS += $(shell sdl2-config --libs)
else
CFLAGS += $(shell sdl-config --cflags) -DSDL12
LDFLAGS += -lm
LDFLAGS += $(shell sdl-config --libs)
ifeq ($(RENDER_GL), 1)
LEGACY_GL = 1
endif
endif

ifeq ($(VANILLA_IS_DEFAULT), 1)
CFLAGS += -DVANILLA_IS_DEFAULT=1
endif

ifeq ($(USE_LOCOLOUR), 1)
CFLAGS += -DUSE_LOCOLOUR=1
endif

ifeq ($(USE_TOONSCAPE), 1)
CFLAGS += -DUSE_TOONSCAPE=1
endif

ifeq ($(RENDER_GL), 1)
SRC += $(wildcard src/gl/*.c src/gl/textures/*.c)
CFLAGS += -I ./cglm/include -DRENDER_GL

ifeq ($(LEGACY_GL), 1)
CFLAGS += -DOPENGL20 #-DOPENGL15
endif

# required for loading texture and sprite sheets
ifeq ($(SDL2), 1)
CFLAGS += $(shell pkg-config --cflags SDL2_image)
LDFLAGS += $(shell pkg-config --libs SDL2_image)
else
CFLAGS += $(shell pkg-config --cflags SDL_image)
LDFLAGS += $(shell pkg-config --libs SDL_image)
endif

# extension wranglers
ifeq ($(GLAD), 1)
CFLAGS += -DGLAD -Iglad -Iglad/glad
SRC += glad/glad.c
else
CFLAGS += $(shell pkg-config --cflags glew)
LDFLAGS += $(shell pkg-config --libs glew)
endif

else
CFLAGS += -DRENDER_SW
endif

# Faster RSA encryption
ifeq ($(WITH_OPENSSL), 1)
CFLAGS += -DWITH_RSA_OPENSSL
CFLAGS += $(shell pkg-config --cflags libcrypto)
LDFLAGS += $(shell pkg-config --libs libcrypto)
endif

ifeq ($(DEBUG), 1)
CFLAGS += -O0 -Wall -Wextra -pedantic -g
#CFLAGS += -fsanitize=undefined
#LDFLAGS += -fsanitize=address -static-libasan
#LDFLAGS += -fsanitize=undefined
else
CFLAGS += -s -O3 -ffast-math
LDFLAGS += -s
endif

all: mudclient

mudclient: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

install: mudclient
	mkdir -p $(DESTDIR)$(PREFIX)/$(BINDIR)
	cp -p mudclient $(DESTDIR)$(PREFIX)/$(BINDIR)
	mkdir -p $(DESTDIR)$(PREFIX)/$(DATADIR)
	cp -Rp cache/* $(DESTDIR)$(PREFIX)/$(DATADIR)
	mkdir -p $(DESTDIR)$(PREFIX)/$(DATAROOTDIR)/applications
	cp -p freedesktop/*.desktop \
		$(DESTDIR)$(PREFIX)/$(DATAROOTDIR)/applications
	mkdir -p $(DESTDIR)$(PREFIX)/$(DATAROOTDIR)/pixmaps
	cp -p web/icon.png \
		$(DESTDIR)$(PREFIX)/$(DATAROOTDIR)/pixmaps/rsc-c.png

clean:
	rm -f src/*.o src/lib/*.o src/lib/rsa/*.o src/ui/*.o
	rm -f src/gl/*.o src/gl/textures/*.o src/custom/*.o glad/*.o
	rm -f mudclient
