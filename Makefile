CC=g++

.PHONY: all

clean:
	rm -f edmonds

all: edmonds

edmonds : edmonds.c edmonds.h
	g++ edmonds.c edmonds.h -o edmonds
