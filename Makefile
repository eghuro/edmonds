bins := edmonds
objs:= edmonds.o

CC=gcc
CFLAGS=-std=g++11

.PHONY: all

clean:
	rm -f *.o $(bins)

all: $(bins)

edmonds.o : edmonds.h

objects: $(objs)
$(objects):%.o:%.c
	$(CC) $(CFLAGS) $< -o $@

edmonds: $(objects)
	$(CC) $(CFLAGS) -o edmonds $(objects)