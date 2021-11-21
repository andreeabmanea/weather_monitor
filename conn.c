#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h> 

int count_lines_from_file(char *path) {
  
  FILE *fp = fopen(path,"r");
  int character = 0;

  if (fp == NULL)
      return 0;

  int number_of_lines = 1;
  while(!feof(fp)) {
      character = fgetc(fp);
      if(character == '\n') {
         number_of_lines++;
      }
   }
  fclose(fp);
  return number_of_lines;
}

int process_file_from_client(sqlite3 *db, char *path) {

   char row[100];
   char *field;
   const char *sql = "INSERT INTO weather(city, calendar_date, min_temperature, max_temperature, precipitations) VALUES (?, ?, ?, ?, ?)";
   sqlite3_stmt *stmt;
   int err = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
   if (err != SQLITE_OK) {
      printf("Prepare for statement failed: %s\n", sqlite3_errmsg(db));
      return;
   }
   FILE *fp;
   fp = fopen(path,"r");
   int last_line = count_lines_from_file(path);
   //printf("Nr of lines:%d\n", lines);   

   int current_line = 0;
   while (!feof(fp)) {
      current_line++;
      fgets(row, 100, fp);
      if (current_line!=last_line)
         row[strlen(row)-1] = '\0';
      // check_row(row);
   
      field = strtok(row, ",");
      int column_index = 1;

      while(field != NULL) {
         field[strlen(field)] = '\0';
         sqlite3_bind_text (stmt, column_index, field, -1, SQLITE_TRANSIENT); 
         field = strtok(NULL, ",");
         column_index++;
         }
         err = sqlite3_step(stmt); 
         if (err != SQLITE_DONE) {
            printf("Execution of the statement failed: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            return;
         }
      sqlite3_reset(stmt);      
      }
   }

int validate_city(char *city) {
   FILE* fp; 
   fp = fopen("valid_cities.txt","r"); 
   char array[100][50];		
   int number_of_strings = 0; 
   while(1) { 
      char character = (char)fgetc(fp); 
      int last = 0; 
      while(character!=',' && !feof(fp)){	
         array[number_of_strings][last++] = character;	
         character = (char)fgetc(fp); 
         } 
        array[number_of_strings][last] = '\0';	
        if(feof(fp)){	
            break; 
        } 
        number_of_strings++; 
    } 
   for(int i = 0;i<=number_of_strings;i++){ 
      if (strcmp(array[i], city))
         return 1;	 
   }
   return 0; 
}



int check_row(char *row_content) {

   char *field = strtok(row_content, ",");
   int column = 1;
   while(field != NULL) {
      field[strlen(field)] = '\0';
      //check every field if it matches a format for the column
      field = strtok(NULL, ",");
      column++;
   }

   return 1;

}

int main() {

   sqlite3 *db;
   char *err_msg = 0;

   int connection;
   connection = sqlite3_open("weather.db", &db);

   if(connection) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return(0);
   } 
      else {
         fprintf(stderr, "Opened database successfully\n");
      }
   process_file_from_client(db, "example.csv");
}




