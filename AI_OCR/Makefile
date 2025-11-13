CC = gcc
CFLAGS = -Wall -Wextra
TARGET = AInxor
SRCS = AInxor.c
OBJS = $(SRCS:.c=.o)
all: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lm

clean:
	rm -f $(OBJS) $(TARGET)
	rm -f network.png
.PHONY: all clean
