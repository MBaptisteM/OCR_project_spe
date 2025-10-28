get_letters : get_letters.o
	gcc get_letters.o -o ocr `sdl2-config --cflags --libs` -lSDL2_image -Wall -Wextra -lm

get_letters.o : get_letters.c
	gcc -c get_letters.c

clean:
	rm -f get_letters.o
	rm -f ocr
.PHONY: all clean
