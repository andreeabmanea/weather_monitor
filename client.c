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
      special_client();
      break;
    default: 
      printf("Unknown port");
  }
  }

  void regular_client(int sd) {
    // while (1) {
      char city[100];
      bzero(city, 100);
      printf("Input a city from Romania \n");
      fflush(stdout);
      fflush(stdin);
      scanf("%s", city);
      write_string_to_socket(sd, city);

      char calendar_date[100];
      bzero(calendar_date, 100);
      printf("Input a date with the format DD/MM/YYYY \n");
      fflush(stdout);
      fflush(stdin);
      scanf("%s", calendar_date);
      write_string_to_socket(sd, calendar_date);

     // receive meteo info from server

      // char exit_msg[100];
      // bzero(exit_msg, 100);
      // printf("Do you wish to disconnect? Types Y/N \n");
      // fflush(stdout);
      // fflush(stdin);
      // scanf("%s", exit_msg);
      // write_string_to_socket(sd, exit_msg);
      // if (strcmp(exit_msg, "Y") == 0) { 
      //   close(sd);
      //   break;
      // }
      
    // }
  }

  void special_client() {

  }