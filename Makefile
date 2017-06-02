CC = gcc
CFLAGS = -Wall -std=gnu99 -pedantic -g
MAIN = pipeline
OBJS = pipeline.o helper.o execute.o

all : $(MAIN)

$(MAIN) : $(OBJS)
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS)

pipeline.o : pipeline.c
	$(CC) $(CFLAGS) -c pipeline.c

helper.o : helper.c helper.h
	$(CC) $(CFLAGS) -c helper.c

execute.o : execute.c execute.h
	$(CC) $(CFLAGS) -c execute.c

clean :
	rm $(MAIN) $(OBJS)
