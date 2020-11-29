CC = gcc
CFLAGS = -Wall -Wextra -pedantic
OBJECTS = main.o

all: pfsim-fifo pfsim-lru pfsim-clock

pfsim-fifo: main.o

pfsim-lru: main.o

pfsim-clock: main.o

main.o: main.c

clean:
    rm -rf $(OBJECTS)
