CC = gcc
CFLAGS = -Wall -Wextra `sdl2-config --cflags` -std=c99
LDFLAGS = `sdl2-config --libs` -lSDL2_ttf -lSDL2_image -lm

SRC = \
	main.c \
	INTERFACE/menu.c \
	INTERFACE/training.c \
	INTERFACE/resolve_window.c \
	READ_GRID/init/Init.c \
	READ_GRID/init/Pixel.c \
	READ_GRID/get_letters/get_letters.c  \
	AI_OCR/training.c \
	AI_OCR/extract_data.c \
	AI_OCR/dict.c \
	SOLVER/Solver.c
OBJ = $(SRC:.c=.o)

menu: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(OBJ) menu solver_grid.txt
