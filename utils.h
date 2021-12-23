#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h> 

void populate_database(sqlite3* db, char* path) {
   
   char table_name[100];
   char field_name[100];
   char field_value[100];

   if (strstr(path, "city") != NULL) {
      strcpy(table_name, "city");
      strcpy(field_name, "city_name");

   }
   else if (strstr(path, "status") != NULL) {
      strcpy(table_name, "weather_status");
      strcpy(field_name, "status");
   }
   
   char sql[100];
   strcpy(sql,"INSERT OR IGNORE INTO ");
   strcat(sql, table_name);
   strcat(sql, "(");
   strcat(sql, field_name);
   strcat(sql, ")");
   strcat(sql, " VALUES (?)");
  
   sqlite3_stmt *stmt;
   int err = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
   if (err != SQLITE_OK) {
      printf("Prepare for statement failed: %s\n", sqlite3_errmsg(db));
      return;
   }
   FILE *fp;
   fp = fopen(path,"r");
   int last_line = count_lines_from_file(path);
   int current_line = 0;
   while (!feof(fp)) {
      current_line++;
      fgets(field_value, 100, fp);
      if (current_line!=last_line)
         field_value[strlen(field_value)-1] = '\0';
      sqlite3_bind_text (stmt, 1, field_value, -1, SQLITE_TRANSIENT); 
      err = sqlite3_step(stmt); 
      if (err != SQLITE_DONE) {
            printf("Execution of the statement failed: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            return;
      }
      sqlite3_reset(stmt);      
      }
}

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
   const char *sql = "INSERT INTO weather_forecast(fk_city, calendar_date, min_temperature, max_temperature, precipitations, fk_status) VALUES (?, ?, ?, ?, ?, ?)";
   sqlite3_stmt *stmt;
   int err = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
   if (err != SQLITE_OK) {
      printf("Prepare for statement failed: %s\n", sqlite3_errmsg(db));
      return;
   }
   FILE *fp;
   fp = fopen(path,"r");
   int last_line = count_lines_from_file(path); 

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

int validate_city(sqlite3 *db, char *city) {
   const char* sql = "SELECT city.city_name FROM city";
   sqlite3_stmt *stmt = NULL;
   
   int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
   if (result != SQLITE_OK) {
      printf("Prepare for statement failed: %s\n", sqlite3_errmsg(db));
      return;
   }
   result = sqlite3_step(stmt);
   char *cities; 
   strcpy(cities, "");

   int row_count = 0;
   while(result == SQLITE_ROW) {
      row_count++;
      int column_count = sqlite3_column_count(stmt);
      for (int column = 0; column < column_count; ++column) {
         strcat(cities, sqlite3_column_text(stmt, column));
         strcat(cities, " ");
      fflush(stdout);
      result = sqlite3_step(stmt);
    }
   }
   result = sqlite3_finalize(stmt);
   if (strstr(cities, city) != NULL)
      return 1;
   return 0;
  }

int validate_weather_status(sqlite3 *db, char *status) {
   const char* sql = "SELECT status FROM weather_status";
   sqlite3_stmt *stmt = NULL;
   
   int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
   if (result != SQLITE_OK) {
      printf("Prepare for statement failed: %s\n", sqlite3_errmsg(db));
      return;
   }
   result = sqlite3_step(stmt);
   char *statuses; 
   strcpy(statuses, "");

   int row_count = 0;
   while(result == SQLITE_ROW) {
      row_count++;
      int column_count = sqlite3_column_count(stmt);
      for (int column = 0; column < column_count; ++column) {
         strcat(statuses, sqlite3_column_text(stmt, column));
         strcat(statuses, " ");
      fflush(stdout);
      result = sqlite3_step(stmt);
    }
   }
   printf("%s\n", statuses);
   result = sqlite3_finalize(stmt);
   if (strstr(statuses, status) != NULL)
      return 1;
   return 0;
  }

char* select_weather_forecast(sqlite3 *db, char* city, char* calendar_date) {
   const char* sql = "SELECT min_temperature, max_temperature, precipitations, fk_status FROM weather_forecast WHERE fk_city = ? and calendar_date = ?";
   sqlite3_stmt *stmt = NULL;
   
   int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
   sqlite3_reset(stmt);
   sqlite3_bind_text(stmt, 1, city, -1, SQLITE_TRANSIENT);
   sqlite3_bind_text(stmt, 2, calendar_date, -1, SQLITE_TRANSIENT);
   if (result != SQLITE_OK) {
      printf("Prepare for statement failed: %s\n", sqlite3_errmsg(db));
      return;
   }
   result = sqlite3_step(stmt);
   
   static char statuses[100];
   strcpy(statuses, "");

   int row_count = 0;
   while(result == SQLITE_ROW) {
      row_count++;
      int column_count = sqlite3_column_count(stmt);
      for (int column = 0; column < column_count; column++) {
         strcat(statuses, sqlite3_column_text(stmt, column));
         strcat(statuses, " ");
         fflush(stdout);
      }
      
      result = sqlite3_step(stmt);
    }
   sqlite3_finalize(stmt);
   return statuses;
  }

int check_credentials(sqlite3 *db, char* username, char* password) {
   const char* sql = "SELECT * FROM user WHERE username = ? and password = ?";
   sqlite3_stmt *stmt = NULL;
   
   int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
   sqlite3_reset(stmt);
   sqlite3_bind_text(stmt, 1, username, -1, SQLITE_TRANSIENT);
   sqlite3_bind_text(stmt, 2, password, -1, SQLITE_TRANSIENT);
   if (result != SQLITE_OK) {
      printf("Prepare for statement failed: %s\n", sqlite3_errmsg(db));
      return;
   }
   result = sqlite3_step(stmt);
   printf("%d\n", result);
   if (result == SQLITE_ROW) {
      return 1;
   }
   sqlite3_finalize(stmt);
   return -1;
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

char* concatenate_database_info(char* city, char* calendar_date, char* min_temperature, char* max_temperature, char* precipitations, char* status) {
	static char db_info[500];
   strcpy(db_info, "");
	bzero(db_info, 500);
	strcat(db_info, "City: ");
	strcat(db_info, city);
	strcat(db_info, "\n");
	strcat(db_info, "Calendar Date: ");
	strcat(db_info, calendar_date);
	strcat(db_info, "\n");
	strcat(db_info, "Min Temperature: ");
	strcat(db_info, min_temperature);
	strcat(db_info, "\n");
	strcat(db_info, "Max Temperature: ");
	strcat(db_info, max_temperature);
	strcat(db_info, "\n");
	strcat(db_info, "Precipitations: ");
	strcat(db_info, precipitations);
	strcat(db_info, "\n");
	strcat(db_info, "Status: ");
	strcat(db_info, status);
   db_info[strlen(db_info)] = '\0';
	return db_info;
}


// int main() {
//    //printf("%lu", hash("andreea"));
//    // printf("%s\n", crypt("andreea","k7"));
//    // printf("%s", crypt("andreea","a7"));
   
//    sqlite3 *db;
//    char *err_msg = 0;

//    int connection;
//    connection = sqlite3_open("weather.db", &db);
//    if(connection) {
//       fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
//       return(0);
//    } 
//       else {
//          fprintf(stderr, "Opened database successfully\n");
//       }
//    // populate_database(db, "database_files/status_for_weather.txt");
//    // process_file_from_client(db, "example.csv");

//    // printf("%d", validate_city(db, "Iasi"));
//    // printf("%d", validate_weather_status(db, "Cloudy"));
//    printf("%s\n",select_weather_forecast(db, "Iasi", "2021-05-20"));
   
// }




