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
    experience INT NOT NULL
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

CREATE TABLE haulages (
    id SERIAL PRIMARY KEY,
    status INT NOT NULL,
    route INT REFERENCES routes(id),
    start_time TIMESTAMP,
    end_time TIMESTAMP
);

CREATE TABLE drv_haul (
    haulage INT REFERENCES haulages(id),
    driver INT REFERENCES users(id),
    driver_number INT,
    driver_bonus MONEY
);


INSERT INTO users
    (fired, role, login, hash, surname, name, patronim, phone, details)
VALUES
    (false, 'logist', 'logist1', MD5('logist'), 'Полудвурельсов', 'Апостол', 'Коромыслович', '+79998887766', NULL),
    (false, 'logist', 'logist2', MD5('logist'), 'Семиколенов', 'Копипаст', NULL, '+79998887765', NULL),
    (true, 'logist', 'logist3', MD5('logist'), 'Стариков', 'Миокардит', 'Самозамесович', '+79998887764', NULL),
    (false, 'driver', 'driver1', MD5('driver'), 'Петров', 'Пётр', 'Петрович', '+79998887763', 'Пьёт'),
    (false, 'driver', 'driver2', MD5('driver'), 'Сидоров', 'Семён', 'Иванович', '+79998887762', 'Не пьёт'),
    (false, 'driver', 'driver3', MD5('driver'), 'Потапов', 'Феоклист', 'Иродович', '+79998887762', 'Ест'),
    (false, 'accounter', 'accounter1', MD5('accounter'), 'Иванов', 'Арсений', 'Полукардович', '+79998887761', NULL)
;

INSERT INTO drv_exp
    (driver, experience)
VALUES
    ( (SELECT id FROM users WHERE login='driver1'), 5),
    ( (SELECT id FROM users WHERE login='driver2'), 4),
    ( (SELECT id FROM users WHERE login='driver3'), 0)
;

INSERT INTO routes 
    (name, active, start_point, end_point, len, details, client_price, drv_fee_base)
VALUES
    ('Рукой подать', true, 'Склад', 'Близко', 10, NULL, 10, 5),
    ('Под боком', true, 'Склад', 'Рядом', 30, '', 30, 10),
    ('Путь-дорожка', true, 'Близко', 'Далеко', 180, NULL, 180, 60),
    ('Дорога дальняя', true, 'Рядом', 'Далеко', 200, NULL, 200, 80),
    ('В глушь', false, 'Оттуда', 'Туда', 800, 'В тех е*енях семеро наших полегло', 950, 350)
;

INSERT INTO haulages  
    (status, route, start_time, end_time)
VALUES
    (1, (SELECT id FROM routes WHERE name='Под боком'), '2023-08-12 10:00', '2023-08-12 12:00'),
    (2, (SELECT id FROM routes WHERE name='Рукой подать'), '2023-08-12 15:32', '2023-08-12 15:51'),
    (0, (SELECT id FROM routes WHERE name='Путь-дорожка'), '2023-08-13 10:20', NULL)
;

INSERT INTO drv_haul 
    (haulage, driver, driver_number, driver_bonus)
VALUES
    ((SELECT id FROM haulages WHERE start_time='2023-08-12 10:00'), (SELECT id FROM users WHERE login='driver1'), 1, 0),
    ((SELECT id FROM haulages WHERE start_time='2023-08-12 15:32'), (SELECT id FROM users WHERE login='driver1'), 1, 0),
    ((SELECT id FROM haulages WHERE start_time='2023-08-13 10:20'), (SELECT id FROM users WHERE login='driver1'), 1, 0),
    ((SELECT id FROM haulages WHERE start_time='2023-08-13 10:20'), (SELECT id FROM users WHERE login='driver2'), 2, 0)
;

CREATE FUNCTION getShortName(user_id INT)
RETURNS VARCHAR
AS
$$
SELECT 
    surname || ' ' || LEFT(name, 1) || '.' || 
        CASE 
            WHEN patronim IS NOT NULL AND patronim != '' THEN
                ' ' || LEFT(patronim, 1) || '.' 
            ELSE 
                '' 
        END  
    FROM users 
    WHERE id = user_id 
    ;
$$
LANGUAGE SQL
;

CREATE FUNCTION getFullName(user_id INT)
RETURNS VARCHAR
AS
$$
SELECT 
    surname || ' ' || name ||  
        CASE 
            WHEN patronim IS NOT NULL AND patronim != '' THEN
                ' ' || patronim  
            ELSE 
                '' 
        END  
    FROM users 
    WHERE id = user_id 
    ;
$$
LANGUAGE SQL
;

