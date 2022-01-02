#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

/* codul de eroare returnat de anumite apeluri */
extern int errno;

char* read_string_from_socket(int sd) {
  int message_length;
  static char buffer[100];

  read(sd, &message_length, sizeof(int));
  read(sd, buffer, message_length);

  return buffer;
}

void write_string_to_socket(int sd, char* message) {
  if (recv(sd, NULL, 1, MSG_PEEK | MSG_DONTWAIT) != 0) {
    int message_length= strlen(message) + 1;
    /* trimiterea mesajului la server */
    write(sd, &message_length, sizeof(int));
    write(sd, message, message_length);
  }
}
/* portul de conectare la server*/
int port;

int main (int argc, char *argv[])
{
  int sd;			// descriptorul de socket
  struct sockaddr_in server;	// structura folosita pentru conectare 

  /* exista toate argumentele in linia de comanda? */
  if (argc != 3)
    {
      printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
      return -1;
    }

  /* stabilim portul */
  port = atoi (argv[2]);

  /* cream socketul */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("Eroare la socket().\n");
      return errno;
    }

  /* umplem structura folosita pentru realizarea conexiunii cu serverul */
  /* familia socket-ului */
  server.sin_family = AF_INET;
  /* adresa IP a serverului */
  server.sin_addr.s_addr = inet_addr(argv[1]);
  /* portul de conectare */
  server.sin_port = htons (port);
  
  /* ne conectam la server */
  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
      perror ("[client]Eroare la connect().\n");
      return errno;
    }

  /* citirea mesajului */
  switch (port) {
    case 2028:
      regular_client(sd);
      break;
    case 2029: 
      special_client(sd);
      break;
    default: 
      printf("Unknown port");
  }
  }

  void regular_client(int sd) {
    while (1) {
      char city[100];
      bzero(city, 100);
      printf("Input a city from Romania \n");
      fflush(stdout);
      fflush(stdin);
      scanf("%s", city);
      write_string_to_socket(sd, city);
      printf("\n");

      char calendar_date[100];
      bzero(calendar_date, 100);
      printf("Input a date with the format YYYY-MM-DD \n");
      fflush(stdout);
      fflush(stdin);
      scanf("%s", calendar_date);
      write_string_to_socket(sd, calendar_date);
      printf("\n");

      // receive meteo info from server
      char response[500];
      bzero(response, 500);
      strcpy(response, read_string_from_socket(sd));
      printf("%s\n", response);
      printf("\n");

      char exit_msg[100];
      bzero(exit_msg, 100);
      printf("Do you wish to disconnect? Types Y/N \n");
      fflush(stdout);
      fflush(stdin);
      scanf("%s", exit_msg);
      write_string_to_socket(sd, exit_msg);
      printf("\n");
      if (strcmp(exit_msg, "Y") == 0) { 
        close(sd);
        break;
      }
      
    }
  }

  void special_client(int sd) {
    while(1) {
      char username[100];
      bzero(username, 100);
      printf("%s","Enter your username: ");
      fflush(stdout);
      fflush(stdin);
      scanf("%s", username);
      write_string_to_socket(sd, username);

      char password[100];
      bzero(password, 100);
      fflush(stdout);
      fflush(stdin);
      strcpy(password, getpass("Enter your password: "));
      write_string_to_socket(sd, password);

      char check[10];
      bzero(check, 10);
      fflush(stdout);
      fflush(stdin);
      strcpy(check, read_string_from_socket(sd));
      if (strcmp(check, "OK") == 0) {
        printf("\nWelcome back, %s!\n", username);
        break;
      }
      else printf("Invalid credentials, please try again!\n\n");
    }

    while (1) {
      FILE *fp;
      char path[100], full_path[200];
      bzero(path,100);
      bzero(full_path, 200);
      printf("\nEnter a file to send to the server:\n");
      scanf("%s", path);
      strcat(full_path, "./client_folder/");
      strcat(full_path, path);
      fp = fopen(full_path, "r");
      char* line;
      size_t len = 0;
      ssize_t read;
      while ((read = getline(&line, &len, fp)) != -1) {
        write_string_to_socket(sd,line);
      }
      write_string_to_socket(sd,"EOF");

      char confirmation[100];
      bzero(confirmation, 100);
      fflush(stdout);
      fflush(stdin);
      strcpy(confirmation, read_string_from_socket(sd));
      printf("%s\n", confirmation);

      char exit_msg[2];
      bzero(exit_msg, 2);
      printf("%s\n", "Do you wish to send another file? Type Y/N");
      fflush(stdout);
      fflush(stdin);
      scanf("%s", exit_msg);
      write_string_to_socket(sd, exit_msg);
      printf("\n");
      if (strcmp(exit_msg, "N") == 0) { 
        close(sd);
        break;
      }
      
    }
  }