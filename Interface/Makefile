menu : main.o menu.o training.o resolve_window.o
	gcc -o menu menu.c main.c training.c resolve_window.c -Wall -Wextra `sdl2-config --cflags --libs` -lSDL2_ttf -lSDL2_image

menu.o : main.c menu.c training.c resolve_window.c
	gcc -c main.c menu.c training.c resolve_window.c
clean:
	rm -f menu.o
	rm -f menu
	rm -f main.o
	rm -f training.o
	rm -f resolve_window.o
.PHONY: all clean