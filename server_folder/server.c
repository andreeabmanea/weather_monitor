#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <crypt.h> 
#include "utils.h"

/* portul folosit */
#define PORT_1 2028
#define PORT_2 2029
extern int errno;
sqlite3 *db;

char* read_string_from_socket(int sd) {
  int message_length;
  static char buffer[100];

  read(sd, &message_length, sizeof(int));
  read(sd, buffer, message_length);

  return buffer;
}

void write_string_to_socket(int sd, char* message) {
    int message_length= strlen(message) + 1;
    /* trimiterea mesajului la server */
    write(sd, &message_length, sizeof(int));
    write(sd, message, message_length);
}



int get_fd_of_accepted_connection(int file_descriptors[], int* which_sd, struct sockaddr *addr, socklen_t *addrlen) {
    
	int maxfd = -1, fd = -1;
    unsigned int i;
    int result; // return of select
	
	fd_set readfds;
	FD_ZERO(&readfds);

	int count = sizeof(file_descriptors)/sizeof(file_descriptors[0]);
    
    for (i = 0; i < count; i++) {
        FD_SET(file_descriptors[i], &readfds);
        if (file_descriptors[i] > maxfd)
            maxfd = file_descriptors[i];
    }
    
	result = select(maxfd + 1, &readfds, NULL, NULL, NULL);
    if (result < 0) {
        return -1;
	}
    
    for (i = 0; i < count; i++)
        if (FD_ISSET(file_descriptors[i], &readfds)) {
            fd = file_descriptors[i];
            break;
        }

	
    if (fd == -1)
        return -1;
    else {
		*which_sd = fd;
        return accept(fd, addr, addrlen);
	}
}

int init_server(int* first_sd, int* second_sd, struct sockaddr_in server, struct sockaddr_in *from) {
	
	int enable = 1;
	// socket family
	server.sin_family = AF_INET;
	// we accept any address
	server.sin_addr.s_addr = htonl (INADDR_ANY);

	bzero (&server, sizeof (server));
    bzero (&from, sizeof (from));

	// first socket
	if ((*first_sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
    	perror ("[server] Error at first socket().\n");
    	return errno;
    }

	// to prevent error at bind (reuse)
	if (setsockopt(*first_sd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    	error("setsockopt(SO_REUSEADDR) failed");
    
    // first port
    server.sin_port = htons (PORT_1);

    // attach first socket
    if (bind (*first_sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
    	perror ("[server] Error at bind() for first socket.\n");
    	return errno;
    }

	// second socket
    if ((*second_sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
    	perror ("[server] Error at second socket().\n");
    	return errno;
    }
	if (setsockopt(*second_sd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    	error("setsockopt(SO_REUSEADDR) failed");
    
    // second port
    server.sin_port = htons (PORT_2);

   // attach the socket
    if (bind (*second_sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
    	perror ("[server]Error at bind() for second socket.\n");
    	return errno;
    }


    // listen for clients
    if (listen (*first_sd, 1) == -1)
    {
    	perror ("[server]Eroare la listen().\n");
    	return errno;
    }

	if (listen(*second_sd, 1) == -1) 
	{
		perror ("[server]Eroare la listen() 2.\n");
		return errno;
	}
}

void treat_regular_client(int client) {
	int exit = 0;

	while (1) {
		if (exit == 0) {

			char city[100];
			bzero(city, 100);
			strcpy(city, read_string_from_socket(client));
			printf ("[server]: Message received...%s\n", city, strlen(city));


			char calendar_date[100];
			bzero(calendar_date, 100);
			strcpy(calendar_date, read_string_from_socket(client));
			printf ("[server]: Message received...%s\n", calendar_date, strlen(calendar_date));


			//send from database the requested info
		
			char db_info[500];
			bzero(db_info, 500);
			strcpy(db_info, select_weather_forecast(db, city, calendar_date));
			
			char* pointer;
   			char min_temp[10];
			char max_temp[10];
			char precipitations[10];
			char status[50];
   			int index = 0;
   			pointer = strtok(db_info, " ");
   			while (pointer!= NULL) {
				pointer[strlen(pointer)] = '\0';
				switch (index) {
					case 0: strcpy(min_temp, pointer);
							break;
					case 1: strcpy(max_temp, pointer);
							break;
					case 2: strcpy(precipitations, pointer);
							break;
					case 3: strcpy(status, pointer);
							break;
					default: printf("End of string");
				}
				index++;
      			pointer = strtok(NULL, " ");
  			}	
			char formatted_info[500];
			strcpy(formatted_info, concatenate_database_info(city, calendar_date, min_temp, max_temp, precipitations, status));
			
			write_string_to_socket(client, formatted_info);
			char* exit_msg;
			exit_msg = read_string_from_socket(client);
			printf ("[server]: Message received...%s\n", exit_msg);
			if (strcmp(exit_msg, "Y") == 0) {
				exit = 1;
				close(client);
			}	
		}
		fflush(stdout);
	}
}

void treat_special_client(int client) {
	char username[100];
	
	while (1) {
		bzero(username, 100);	
		strcpy(username, read_string_from_socket(client));
		printf ("[server]: Message received...%s\n", username, strlen(username));

		char password[100];
		bzero(password, 100);	
		strcpy(password, read_string_from_socket(client));
		printf ("[server]: Message received...%s\n", password, strlen(password));

		char encrypted_pass[200];
		bzero(encrypted_pass, 200);
		strcpy(encrypted_pass, crypt(password,"k7"));

		if (check_credentials(db, username, encrypted_pass) == 1) {
			write_string_to_socket(client, "OK");
			break;
		}
		else write_string_to_socket(client, "Not OK");
	}

	while (1) {
		char path[100] = "./server_folder/file_from_";
		strcat(path,username);
		strcat(path,".csv");
		FILE *fp;
		fp = fopen(path, "w");
		char file_line[1000];
		while (1) {
			bzero(file_line,1000);
			strcpy(file_line,read_string_from_socket(client));
			if (strcmp(file_line, "EOF") != 0) {
				fputs(file_line, fp);
			} else {
				close(fp);
				fflush(fp);
				process_file_from_client(db, path, username);
				break;
			}
		}
		char* exit_msg;
		exit_msg = read_string_from_socket(client);
		printf ("[server]: Message received...%s\n", exit_msg);
		if (strcmp(exit_msg, "N") == 0) {
			close(client);
			break;
		}	
	}
}

int main ()
{
	
	char *err_msg = 0;

	
	int first_sd, second_sd;	
	struct sockaddr_in server;
    struct sockaddr_in from;
	init_server(&first_sd, &second_sd, server, &from);
    
	int socket_descriptors[] = {first_sd, second_sd};
    
	fd_set readfds;
    int client;
	int result;
        
	while (1)
    {
    	
    	int length = sizeof (from);
		int which_sd; // which socket did the connection happen on
    	printf ("Waiting for clients\n");
    	fflush (stdout);
    	
		/* we accept a client (blocking call) */
		
    	client = get_fd_of_accepted_connection(socket_descriptors, &which_sd, (struct sockaddr *) &from, &length);
    	if (client < 0)
    	{
    		perror ("Error at accept().\n");
    		continue;
    	}
    	int pid;
    	if ((pid = fork()) == -1) {
    		close(client);
    		continue;
    	} else if (pid > 0) {
    		// parinte
    		close(client);
    		while(waitpid(-1,NULL,WNOHANG));
    		continue;
    	} else if (pid == 0) {
    		// copil
    		close(first_sd);
			int connection;
			connection = sqlite3_open("weather.db", &db);
			if(connection) {
				fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
				return(0);
			} 
			else {
			fprintf(stderr, "Opened database successfully\n");
			}

    		/* s-a realizat conexiunea, se astepta mesajul */
    	
    		printf ("[server] The client connected at port %d...\n", (which_sd == 3) ? 2028 : 2029);
    		fflush (stdout);
			switch (which_sd) {
				case 3: 
					treat_regular_client(client);
					break;
				case 4:
					treat_special_client(client);
					break;
				default:
					printf("Socket unknown");
			}

    		exit(0);
    	}

    }		
}			

