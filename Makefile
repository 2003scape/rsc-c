SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
LFLAGS = -lSDL

mudclient: $(OBJ)
	cc -o $@ $^ $(LFLAGS)

clean:
	rm -f src/*.o
	rm -f mudclient
