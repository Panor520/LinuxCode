

all:server client

server:
	gcc server.c ../lib/customsocket.c -o server -Wall

client:
	gcc client.c ../lib/customsocket.c -o client -Wall

clean: 
	-rm -rf server
	-rm -rd client

.PHONY:clean all

