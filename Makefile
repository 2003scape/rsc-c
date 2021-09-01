#CC = clang
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
CFLAGS = -Wall -Wextra -pedantic -fPIE -g
LDFLAGS = -lm -lSDL2

mudclient: $(OBJ)
	cc -o $@ $^ $(LDFLAGS)

clean:
	rm -f src/*.o
	rm -f mudclient
