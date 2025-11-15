menu : main.o menu.o
	gcc -o menu menu.c main.c -Wall -Wextra `sdl2-config --cflags --libs` -lSDL2_ttf -lSDL2_image

menu.o : main.c menu.c
	gcc -c main.c menu.c
clean:
	rm -f menu.o
	rm -f menu
	rm -r main.o
.PHONY: all clean