#fisier folosit pentru compilarea serverului&clientului TCP iterativ

all:
	gcc server_folder/server.c -o server_folder/server -lsqlite3 -lcrypt
	gcc test.c -o test -lsqlite3
	gcc client_folder/client.c -o client_folder/client
clean:
	rm -f client server