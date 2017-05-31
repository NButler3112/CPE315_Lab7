CC = gcc
CFLAGS = -Wall -std=gnu99 -pedantic -g
MAIN = pipeline
OBJS = pipeline.o readline.o

all : $(MAIN)

$(MAIN) : $(OBJS)
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS)

pipeline.o : pipeline.c pipeline.h
	$(CC) $(CFLAGS) -c pipeline.c

readline.o : readline.c readline.h
	$(CC) $(CFLAGS) -c readline.c

clean :
	rm $(MAIN) $(OBJS)
