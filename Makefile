# Makefile — fixed (links math library)
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDLIBS = -lm

# Source and object files (explicit order)
SRC = src/graph.c src/hash.c src/queue.c src/reservation.c
OBJ = $(SRC:.c=.o)

TARGET = airline

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ) main.o
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) main.o $(LDLIBS)

# Compile main
main.o: main.c include/graph.h include/hash.h include/queue.h include/reservation.h
	$(CC) $(CFLAGS) -c main.c

# Compile each source file with proper header dependencies
src/graph.o: src/graph.c include/graph.h
	$(CC) $(CFLAGS) -c src/graph.c -o src/graph.o

src/hash.o: src/hash.c include/hash.h include/utils.h
	$(CC) $(CFLAGS) -c src/hash.c -o src/hash.o

src/queue.o: src/queue.c include/queue.h include/hash.h
	$(CC) $(CFLAGS) -c src/queue.c -o src/queue.o

src/reservation.o: src/reservation.c include/reservation.h include/hash.h include/queue.h include/graph.h
	$(CC) $(CFLAGS) -c src/reservation.c -o src/reservation.o

clean:
	rm -f $(OBJ) main.o $(TARGET)
