CC = gcc
CCFLAGS = -Wall -Wshadow
LFLAGS = -lm

.PHONY = all clean debug

all: main

debug: CCFLAGS += -O0 -g
debug: main

main: main.o tinyexpr.o
	$(CC) $(CCFLAGS) -o $@ $^ $(LFLAGS)

.c.o:
	$(CC) -c $(CCFLAGS) $< -o $@

clean:
	rm -f *.o *.exe main
