FLAGS = -Wall -lpthread
SRCS = $(wildcard *.c)
OBJS = ${SRCS:.cpp=.o}

.PHONY: all clean mrproper

all: main

main: main.c $(OBJS)
	g++ $(FLAGS) $(OBJS) -o main

clean:
	rm -rf *.o *~ main
