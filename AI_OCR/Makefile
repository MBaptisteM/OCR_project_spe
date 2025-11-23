CC = gcc
CFLAGS = -O3 -std=c99 `sdl2-config --cflags --libs` -lSDL2_image -Wall -Wextra -lm

SRC = main.c training.c extract_data.c dict.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

all: main

main: ${OBJ}
	${CC} -o $@ $^ ${CFLAGS}

training: training.c
	${CC} -o training training.c ${LDFLAGS}

-include ${DEP}

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} training.o
	${RM} main.o
	${RM} main
