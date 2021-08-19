CC = g++
FLAGS = -std=c++11 -Wall -pedantic
OBJS = main.o

all: $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o main
	
main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

clean:
	rm -f main $(OBJS)
