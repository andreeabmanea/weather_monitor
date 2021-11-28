CREATE TABLE weather_forecast(fk_city TEXT, calendar_date TEXT, min_temperature TEXT, max_temperature TEXT, precipitations TEXT, fk_status TEXT, updated_by TEXT);
CREATE TABLE city(city_name TEXT);
CREATE TABLE weather_status(status TEXT);
CREATE TABLE user(username TEXT, password TEXT);