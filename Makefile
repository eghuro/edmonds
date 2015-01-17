CC=g++

.PHONY: all

clean:
	rm -f edmonds edmonds-test

all: edmonds

edmonds : edmonds.c edmonds.h
	$(CC) edmonds.c edmonds.h -o edmonds

test: test.cpp edmonds.h
	$(CC) test.cpp edmonds.h -o edmonds-test
