#fisier folosit pentru compilarea serverului&clientului TCP iterativ

all:
	gcc server.c -o server
	gcc client.c -o client
	gcc conn.c -o conn -lsqlite3
clean:
	rm -f client server