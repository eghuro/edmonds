CC=g++
CFLAGS=-c -Wall -std=c++11
LDFLAGS=-Wall

objs := graph.o finder.o test.o
prog := test

.PHONY: all

clean:
	rm -f ${objs} ${prog}

all: $(prog)

${prog}: $(objs)
	${CC} ${LDFLAGS} -o $@ $^

%.o:%.cpp
	$(CC) $(CFLAGS) -o $@ $<

${objs} : edmonds.h



