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
    printf("%s\n", select_weather_forecast(db, "Iasi", "2021-05-20"));
    printf("-----------------------\n");
    printf("%s\n", select_weather_forecast(db, "Iasi", "2021-05-20"));
    printf("-----------------------\n");
    printf("%s\n", select_weather_forecast(db, "Iasi", "2021-05-20"));
  

}