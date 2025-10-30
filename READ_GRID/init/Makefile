init : Init.o Pixel.o
	gcc Init.o Pixel.o -o init -std=c99 -Wall -Wextra `sdl2-config --cflags --libs` -lSDL2_image -lm

Init.o : Init.c
	gcc -c Init.c -std=c99 -Wall -Wextra

Pixel.o : Pixel.c
	gcc -c Pixel.c -std=c99 -Wall -Wextra

clean:
	rm -f Init.o Pixel.o
	rm -f init

.PHONY: all clean