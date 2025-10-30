get_letters : main.o get_letters.o
	gcc main.o get_letters.o -o ocr `sdl2-config --cflags --libs` -lSDL2_image -Wall -Wextra -lm

get_letters.o : main.c get_letters.c
	gcc -c main.c get_letters.c

clean:
	rm -f get_letters.o
	rm -f ocr
	rm -r main.o
.PHONY: all clean
