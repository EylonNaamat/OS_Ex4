stack_server.o: stack_server.cpp
	g++ -c stack_server.cpp

stack_server: stack_server.o
	g++ stack_server.o -o stack_server -pthread

client.o: client.cpp
	g++ -c client.cpp

client: client.o
	g++ client.o -o client

tests.o: tests.cpp
	g++ -c tests.cpp

tests: tests.o
	g++ tests.o -o tests

all: stack_server client tests

.PHONY: clean

clean:
	rm *.o stack_server client tests

