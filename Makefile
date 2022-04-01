#CC = clang
#DEBUG = 1
SRC = $(wildcard src/*.c src/ui/*.c)
OBJ = $(SRC:.c=.o)
CFLAGS = -DREVISION_177
LDFLAGS = -lm -lSDL2

ifdef DEBUG
CFLAGS += -Wall -Wextra -pedantic -g
LDFLAGS += -fsanitize=address -static-libasan
else
CFLAGS += -s -Ofast
LDFLAGS += -s
endif

mudclient: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f src/*.o src/ui/*.o
	rm -f mudclient
