SHELL = /bin/bash
all:
	g++ client.cpp -o client -pthread
	g++ server.cpp -o server
	
clean: Makefile funcs.hpp client.cpp server.cpp
	bash -c "rm -v !("Makefile"|"funcs.hpp"|"client.cpp"|"server.cpp")"