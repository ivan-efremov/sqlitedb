CXX      = g++
CC       = gcc
CFLAGS   = -ggdb -Isqlite -Wall -O2 -DSQLITE_OS_UNIX=1
CXXFLAGS = $(CFLAGS) -std=c++11
LDFLAGS  = -D_REENTRANT -pthread -Wl,--no-as-needed -ldl -lz

all: test

test: sqlite3.o main.o
	$(CXX) $(LDFLAGS) sqlite3.o main.o -o test

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

sqlite3.o: sqlite/sqlite3.c
	$(CC) $(CFLAGS) -c sqlite/sqlite3.c -o sqlite3.o

.PHONY: clean
clean:
	-rm -rf *.o test
