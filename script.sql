-- Database: CalcDatabase
-- DROP DATABASE "CalcDatabase";

CREATE DATABASE "CalcDatabase"
    WITH 
    OWNER = postgres
    ENCODING = 'UTF8'
    LC_COLLATE = 'ru_RU.UTF-8'
    LC_CTYPE = 'ru_RU.UTF-8'
    TABLESPACE = pg_default
    CONNECTION LIMIT = -1;
	
CREATE TABLE users (
	id          		BIGSERIAL NOT NULL PRIMARY KEY,
	login			text UNIQUE NOT NULL,
	password   		text NOT NULL,
	account_balance		INTEGER NOT NULL
);

CREATE TABLE sessions (
	id 			BIGSERIAL NOT NULL PRIMARY KEY,
	user_id 		INTEGER REFERENCES users NOT NULL,
	date    		timestamp without time zone NOT NULL,
	expression 		text NOT NULL,
	result_of_expression 	text NOT NULL
);

INSERT INTO users(login, password, account_balance) 
VALUES 	('belousotroll', 'pass', 15),
	   	('gladkikh', 'daniil', 10),
		('sappyk', 'sappyk', 5);

