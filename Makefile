CC=g++

.PHONY: all

clean:
	rm -f edmonds

all: edmonds

edmonds : edmonds.c edmonds.h
	g++ -Wall edmonds.c edmonds.h -o edmonds

test: test.cpp edmonds.h
	g++ -Wall test.cpp edmonds.h -o edmonds-test