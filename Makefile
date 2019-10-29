CC=g++
CFLAGS=-g -c
LFLAGS=

build: a.out

Calculator.o: Calculator.cpp
	$(CC) $(CFLAGS) Calculator.cpp -o Calculator.o

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp -o main.o

a.out: Calculator.o main.o
	$(CC) $(FLAGS) Calculator.o main.o -o a.out

clean:
	rm -rf Calculator.o
	rm -rf main.o
	rm -rf a.out

rebuild: clean build
