DROP DATABASE IF EXISTS kp4db WITH (FORCE);
CREATE DATABASE kp4db;
\connect kp4db;


CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    fired BOOLEAN NOT NULL,
    role VARCHAR NOT NULL,
    login VARCHAR NOT NULL,
    hash VARCHAR(32) NOT NULL,
    surname VARCHAR NOT NULL,
    name VARCHAR NOT NULL,
    patronim VARCHAR,
    phone VARCHAR(16) NOT NULL,
    details VARCHAR
);

CREATE TABLE drv_exp (
    driver INT REFERENCES users(id),
    expirience INT NOT NULL
);

CREATE TABLE routes (
    id SERIAL PRIMARY KEY,
    name VARCHAR NOT NULL,
    active BOOLEAN NOT NULL,
    start_point VARCHAR NOT NULL,
    end_point VARCHAR NOT NULL,
    len INT NOT NULL,
    details VARCHAR,
    client_price MONEY,
    drv_fee_base MONEY
);

CREATE TABLE transportations (
    id SERIAL PRIMARY KEY,
    status INT NOT NULL,
    route INT REFERENCES routes(id),
    start_time TIMESTAMP,
    end_time TIMESTAMP
);

CREATE TABLE drv_transp (
    transp INT REFERENCES transportations(id),
    driver INT REFERENCES users(id),
    driver_bonus MONEY
);


GRANT CONNECT ON DATABASE kp4db TO name_checker;
GRANT CONNECT ON DATABASE kp4db TO driver;
GRANT CONNECT ON DATABASE kp4db TO accounter;
GRANT CONNECT ON DATABASE kp4db TO logist;

GRANT SELECT ON users TO name_checker;

GRANT SELECT ON routes TO driver;
GRANT SELECT ON transportations TO driver;
GRANT SELECT ON drv_exp TO driver;
GRANT SELECT ON drv_transp TO driver;

GRANT SELECT, INSERT ON users TO accounter;
GRANT SELECT, INSERT ON drv_exp TO accounter;

GRANT SELECT ON users TO logist;
GRANT SELECT ON drv_exp TO logist;
GRANT SELECT, INSERT, UPDATE ON routes TO logist;
GRANT SELECT, INSERT, UPDATE ON transportations TO logist;
GRANT SELECT, INSERT, UPDATE ON drv_transp TO logist;


INSERT INTO users
    (fired, role, login, hash, surname, name, patronim, phone, details)
VALUES
    (false, 'logist', 'logist1', '', 'Первый', 'Логист', 'С отчеством', '+79998887766', NULL),
    (false, 'logist', 'logist2', '', 'Второй', 'Логист', '', '+79998887765', NULL),
    (true, 'logist', 'logist3', '', 'Третий', 'Логист', 'Уволен', '+79998887764', NULL),
    (false, 'driver', 'driver1', '', 'Первый', 'Водитель', 'С отчеством', '+79998887763', NULL),
    (false, 'driver', 'driver2', '', 'Второй', 'Водитель', 'С отчеством', '+79998887762', NULL),
    (false, 'accounter', 'accounter1', '', 'Первый', 'Бухгалтер', 'С отчеством', '+79998887761', NULL)
;

INSERT INTO drv_exp
    (driver, expirience)
VALUES
    ( (SELECT id FROM users WHERE login='driver1'), 2),
    ( (SELECT id FROM users WHERE login='driver2'), 4)

