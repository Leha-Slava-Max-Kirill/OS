CC = gcc
CFLAGS = -std=gnu99 -pedantic --no-warnings -Wall -g -lzmq -pthread
SRC_PARENT = server.c
SRC_CHILD = client.c
PROG_PARENT = server
PROG_CHILD = client
FILE_LOGINS = logins.txt

build:
	$(CC) $(SRC_PARENT) $(CFLAGS) -o $(PROG_PARENT) && $(CC) $(SRC_CHILD) $(CFLAGS) -o $(PROG_CHILD)

clean:
	rm $(PROG_PARENT) $(PROG_CHILD) $(FILE_LOGINS)
