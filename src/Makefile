CC=g++
CFLAGS=-c -Wall -std=c++11 -O2 -g -pg
LDFLAGS=-Wall

objs := graph.o finder.o dispatch.o
prog := test

.PHONY: all

all: $(prog)

${prog}: $(objs)
	${CC} ${LDFLAGS} -o $@ $^

%.o:%.cpp
	$(CC) $(CFLAGS) -o $@ $<

${objs} : edmonds.h

clean:
	rm -f ${objs} ${prog}

