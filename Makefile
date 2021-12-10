#fisier folosit pentru compilarea serverului&clientului TCP iterativ

all:
	gcc server.c -o server -lsqlite3
	gcc client.c -o client
clean:
	rm -f client server