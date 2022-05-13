CC = gcc
AR = ar
CFLAGS = -g -Wall -L. -pthread -fopenmp -Wl,--no-as-needed -ldl -DDEBUG

all: libqueue.a libTeslaFactory.a test.out

# Compile queue.c and queue.h
queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c $^

# Convert queue.o into static library.
libqueue.a: queue.o
	$(AR) rcs $@ $^

robot.o: robot.h robot_internal.h robot.c
	$(CC) $(CFLAGS) -c $^

production.o: production.h production.c
	$(CC) $(CFLAGS) -c $^

utils.o: utils.h utils.c
	$(CC) $(CFLAGS) -c $^

libTeslaFactory.a: robot.o utils.o production.o
	$(AR) rcs $@ $^

test.out: test.c
	$(CC) $(CFLAGS) -o $@ $^ -lTeslaFactory -lqueue

.PHONY: clean

clean:
	rm -f *.o *.out *.a *.gch
