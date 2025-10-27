CC = gcc
CFLAGS = ocr `sdl2-config --cflags --libs` -lSDL2_image -Wall -Wextra
TARGET = get_letters
SRCS = get_letters.c
OBJS = $(SRCS:.c=.o)
all: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) $^ -o $(CFLAGS) -lm

clean:
	rm -f $(OBJS) $(TARGET)
	rm -f ocr
.PHONY: all clean
