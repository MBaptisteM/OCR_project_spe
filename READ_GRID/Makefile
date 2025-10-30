CC = gcc
CFLAGS = -std=c99 `sdl2-config --cflags --libs` -lSDL2_image -Wall -Wextra -lm

SRC = main.c get_letters/get_letters.c init/Init.c init/Pixel.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

all: main

main: ${OBJ}
	${CC} -o $@ $^ ${CFLAGS}

operations: operations.c
	${CC} -o operations operations.c ${LDFLAGS}

-include ${DEP}

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} get_letters.o
	${RM} Init.o
	${RM} Pixel.o
	${RM} main.o
