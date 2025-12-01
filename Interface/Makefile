CC = gcc
CFLAGS = -Wall -Wextra `sdl2-config --cflags` -std=c99
LDFLAGS = `sdl2-config --libs` -lSDL2_ttf -lSDL2_image -lm

SRC = main.c menu.c training.c resolve_window.c ../READ_GRID/init/Init.c ../READ_GRID/init/Pixel.c ../READ_GRID/get_letters/get_letters.c 
OBJ = $(SRC:.c=.o)

menu: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f $(OBJ) menu
