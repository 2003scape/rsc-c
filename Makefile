#CC = clang
DEBUG = 1
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
CFLAGS = -fPIE

ifdef DEBUG
CFLAGS = -Wall -Wextra -pedantic -g
else
CFLAGS = -s -Ofast
endif
LDFLAGS = -lm -lSDL2

mudclient: $(OBJ)
	cc -o $@ $^ $(LDFLAGS)

clean:
	rm -f src/*.o
	rm -f mudclient
