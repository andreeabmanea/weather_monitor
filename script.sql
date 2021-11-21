CREATE TABLE country(id INT, name TEXT);
CREATE TABLE city(id INT, name TEXT, fk_country_id INT);
CREATE TABLE weather(id INT, fk_city_id INT, calendar_date DATE);