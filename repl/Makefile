.PHONY: debug build example

STD=-std=c11
FLAGS=-c -W -Wall -pedantic

debug: FLAGS += -g -O0
debug: build

build: bin/main

bin/main: bin library/lib/libcgraph.a main.c example
	gcc $(STD) -o bin/main main.c -Ilibrary/include $(FLAGS)
	gcc $(STD) -o bin/main main.c -Ilibrary/include -Llibrary/lib -lcgraph -g -O0

library/lib/libcgraph.a: library
ifeq (,$(wildcard ./library/lib/libcgraph.a))
	curl -O -L https://github.com/Lds20k/c-graph-library/releases/download/v1.0.2/cgraph.tar.gz
	tar -xvf cgraph.tar.gz -C library
	rm cgraph.tar.gz
endif

example: bin
ifeq (,$(wildcard ./bin/main.bcc))
	cp main.bcc bin/main.bcc
endif

library:
	mkdir library -p

bin:
	mkdir bin -p

clean:
	rm -rf bin
	rm -rf library
