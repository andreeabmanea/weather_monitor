all:
	gcc server_folder/server.c -o server_folder/server -lsqlite3 -lcrypt
	gcc client_folder/client.c -o client_folder/client
clean:
	rm -f client_folder/client server_folder/server