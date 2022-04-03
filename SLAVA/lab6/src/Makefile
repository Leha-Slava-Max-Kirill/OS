CC = g++
CFLAGS = -pedantic --no-warnings -Wall -g -lzmq -pthread
SRC_PARENT = server.cpp
SRC_CHILD = client.cpp
PROG_PARENT = server
PROG_CHILD = client

build:
	$(CC) $(SRC_PARENT) $(CFLAGS) -o $(PROG_PARENT) && $(CC) $(SRC_CHILD) $(CFLAGS) -o $(PROG_CHILD)

clean:
	rm $(PROG_PARENT) $(PROG_CHILD) 