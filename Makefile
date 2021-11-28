#fisier folosit pentru compilarea serverului&clientului TCP iterativ

all:
	gcc server.c -o server
	gcc client.c -o client
	gcc utils.c -o utils -lsqlite3 -lcrypt
clean:
	rm -f client server