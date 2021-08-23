SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
CFLAGS = -fPIE -Wall
LDFLAGS = -lm -lSDL

mudclient: $(OBJ)
	cc -o $@ $^ $(LDFLAGS)

clean:
	rm -f src/*.o
	rm -f mudclient
