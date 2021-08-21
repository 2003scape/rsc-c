SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
CFLAGS = -fPIE -Wall
LFLAGS = -lm -lSDL

mudclient: $(OBJ)
	cc -o $@ $^ $(LFLAGS)

clean:
	rm -f src/*.o
	rm -f mudclient
