#include "server_folder/utils.h"
#include <sqlite3.h>
int main() {
    sqlite3* db;
    int connection;
    connection = sqlite3_open("weather.db", &db);
    if(connection) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(0);
    } 
    else {
    fprintf(stderr, "Opened database successfully\n");
    }
   char username[100];
   strcpy(username,"andreea");
   process_file_from_client(db, "server_folder/file_from_andreea.csv", username);
}