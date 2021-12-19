#include "utils.h"
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
    char city[100];
    char cal_date[100];
    char mini[2];
    char maxi[2];
    char prep[10];
    char status[10];
    strcpy(city, "Iasi"); 
    strcpy(cal_date, "2021-05-20");
     strcpy(mini, "2"); 
    strcpy(maxi, "3");
     strcpy(prep, "Iasi"); 
    strcpy(status, "-20");
    char res[500];
    strcpy(res, concatenate_database_info(city, cal_date, mini, maxi, prep, status));
   printf("%s\n", res);
}