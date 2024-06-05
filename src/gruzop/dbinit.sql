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
CREATE UNIQUE INDEX unique_user_login_idx ON users(login) WHERE fired = false;
CREATE UNIQUE INDEX unique_user_phone_idx ON users(phone) WHERE fired = false;

CREATE TABLE drv_exp (
    driver INT REFERENCES users(id),
    experience INT NOT NULL,
    UNIQUE(driver)
);

CREATE TABLE routes (
    id SERIAL PRIMARY KEY,
    name VARCHAR NOT NULL CHECK(name != ''),
    active BOOLEAN NOT NULL,
    start_point VARCHAR NOT NULL CHECK(start_point != ''),
    end_point VARCHAR NOT NULL CHECK(end_point != ''),
    len INT NOT NULL,
    details VARCHAR,
    client_price MONEY,
    drv_fee_base MONEY
);
CREATE UNIQUE INDEX unique_route_name_idx ON routes(name) WHERE active = true;

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
    driver_bonus MONEY,
    PRIMARY KEY(haulage, driver)
);


INSERT INTO users
    (fired, role, login, hash, surname, name, patronim, phone, details)
VALUES
    (false, 'logist', 'logist1', MD5('logist'), 'Полудвурельсов', 'Апостол', 'Коромыслович', '+79998887766', NULL),
    (false, 'logist', 'logist2', MD5('logist'), 'Семиколенов', 'Копипаст', NULL, '+79998887765', NULL),
    (true, 'logist', 'logist3', MD5('logist'), 'Стариков', 'Миокардит', 'Самозамесович', '+79998887764', NULL),
    (false, 'driver', 'driver1', MD5('driver'), 'Петров', 'Пётр', 'Петрович', '+79998887763', 'Пьёт'),
    (false, 'driver', 'driver2', MD5('driver'), 'Сидоров', 'Семён', 'Иванович', '+79998887762', 'Не пьёт'),
    (false, 'driver', 'driver3', MD5('driver'), 'Потапов', 'Феоклист', 'Иродович', '+79998887761', 'Ест'),
    (false, 'accounter', 'accounter1', MD5('accounter'), 'Иванов', 'Арсений', 'Полукардович', '+79998887760', NULL)
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

CREATE FUNCTION checkPassword(user_id INT, user_hash VARCHAR)
RETURNS BOOLEAN
AS $$
    SELECT EXISTS (
        SELECT id
        FROM users 
        WHERE 
            id = user_id 
            AND
            hash = user_hash
    )
    ;
$$ LANGUAGE SQL
;

CREATE FUNCTION getLoginData(user_login VARCHAR, user_hash VARCHAR)
RETURNS TABLE (
    id INT,
    role VARCHAR
)
AS $$
    SELECT 
        id,
        role
    FROM users 
    WHERE 
        login = user_login 
        AND
        hash = user_hash
        AND
        fired = false
    ;
$$ LANGUAGE SQL
;

CREATE FUNCTION getShortName(user_id INT)
RETURNS VARCHAR
AS $$
    SELECT 
        CONCAT(surname, ' ', LEFT(name, 1), '.') 
            || 
            CASE 
                WHEN patronim IS NOT NULL AND patronim != '' THEN
                    CONCAT(' ',  LEFT(patronim, 1), '.' )
                ELSE 
                    '' 
            END  
    FROM users 
    WHERE id = user_id 
    ;
$$ LANGUAGE SQL
;

CREATE FUNCTION getFullName(user_id INT)
RETURNS VARCHAR
AS $$
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
$$ LANGUAGE SQL
;

CREATE FUNCTION getUsers(users_role VARCHAR)
RETURNS TABLE (
    id INT,
    Фамилия VARCHAR,
    Имя VARCHAR,
    Отчество VARCHAR,
    Телефон VARCHAR,
    Примечание VARCHAR
) AS $$
    SELECT
        id, 
        surname, 
        name, 
        patronim,
        phone,
        details
    FROM
        users
        LEFT JOIN drv_exp ON users.id = drv_exp.driver
    WHERE
        role = users_role
        AND
        fired = false
    ;
$$ LANGUAGE SQL
;

CREATE FUNCTION getDrivers()
RETURNS TABLE (
    id INT,
    Фамилия VARCHAR,
    Имя VARCHAR,
    Отчество VARCHAR,
    Стаж VARCHAR,
    Телефон VARCHAR,
    Примечание VARCHAR
) AS $$
    SELECT
        id, 
        surname, 
        name, 
        patronim,
        experience,
        phone,
        details
    FROM
        users
        LEFT JOIN drv_exp ON users.id = drv_exp.driver
    WHERE
        role = 'driver'
        AND
        fired = false
    ;
$$ LANGUAGE SQL
;

CREATE FUNCTION getUsersShort(users_role VARCHAR)
RETURNS TABLE (
    id INT, name VARCHAR
) AS $$
    SELECT id, getFullName(id)
    FROM users 
    WHERE role = users_role
    ORDER BY id
    ;
$$ LANGUAGE SQL
;

CREATE FUNCTION getUsersShortWithEmpty(users_role VARCHAR)
RETURNS TABLE (
    id INT, name VARCHAR
) AS $$
    SELECT 0 as id, '<Никто не выбран>' as name
    UNION
    SELECT * 
    FROM getUsersShort(users_role)
    ORDER BY id
    ;
$$ LANGUAGE SQL
;

CREATE FUNCTION getActiveRoutes()
RETURNS TABLE (
    id INT,
    Название VARCHAR,
    Откуда VARCHAR,
    Куда VARCHAR,
    "Длина (км)" INT,
    Цена VARCHAR,
    Примечание VARCHAR
) AS $$
    SELECT
        id,
        name,
        start_point,
        end_point,
        len,
        client_price,
        details
    FROM routes 
    WHERE active = true
    ORDER BY 
        active DESC,
        start_point,
        end_point,
        name
    ;
$$ LANGUAGE SQL
;

CREATE FUNCTION getActiveRoutesShort()
RETURNS TABLE (
    id INT,
    description VARCHAR
) AS $$
    SELECT
        id, 
        CONCAT( 
            name, ' (', start_point, ' -> ', end_point, ', ', len, ' км', ')' 
        ) 
    FROM routes 
    WHERE active = true 
    ORDER BY 
        name, 
        start_point, 
        end_point 
    ;
$$ LANGUAGE SQL
;

CREATE FUNCTION getAllRoutes()
RETURNS TABLE (
    id INT,
    Статус VARCHAR,
    Название VARCHAR,
    Откуда VARCHAR,
    Куда VARCHAR,
    "Длина (км)" INT,
    Цена VARCHAR,
    Примечание VARCHAR
) AS $$
    SELECT
        id,
        CASE 
            WHEN active = true THEN 'Открыт' 
            WHEN active = false THEN 'Закрыт' 
            ELSE 'Неизвестно' 
        END, 
        name,
        start_point,
        end_point,
        len,
        client_price,
        details
    FROM routes 
    ORDER BY 
        active DESC,
        start_point,
        end_point,
        name
    ;
$$ LANGUAGE SQL
;

CREATE FUNCTION getRouteInfo(route_id INT)
RETURNS TABLE (
    id INT,
    active BOOLEAN,
    name VARCHAR,
    start_point VARCHAR,
    end_point VARCHAR,
    len INT,
    client_price INT,
    drv_fee_base INT,
    details VARCHAR
) AS $$
    SELECT
        id,
        active,
        name,
        start_point,
        end_point,
        len,
        client_price::numeric,
        drv_fee_base::numeric,
        details
    FROM routes 
    WHERE id = route_id
    ;
$$ LANGUAGE SQL
;

CREATE FUNCTION getHaulages()
RETURNS TABLE (
    id int, 
    Статус VARCHAR, 
    Начато TIMESTAMP, 
    Завершено TIMESTAMP, 
    Маршрут VARCHAR, 
    Откуда VARCHAR, 
    Куда VARCHAR, 
    "Водитель 1" VARCHAR, 
    "Водитель 2" VARCHAR 
) AS $$
    SELECT 
        haulages.id, 
        CASE 
            WHEN haulages.status = 0 THEN 'В работе' 
            WHEN haulages.status = 1 THEN 'Завершено' 
            WHEN haulages.status = 2 THEN 'Отменено' 
            ELSE 'Неизвестно' 
        END, 
        haulages.start_time, 
        haulages.end_time, 
        routes.name, 
        routes.start_point, 
        routes.end_point, 
        getShortName(du1.id), 
        getShortName(du2.id) 
    FROM haulages 
    JOIN routes 
        ON haulages.route = routes.id 
    JOIN drv_haul dh1 
        ON dh1.haulage = haulages.id AND dh1.driver_number = 1 
    JOIN users du1 
        ON du1.id = dh1.driver 
    LEFT JOIN drv_haul dh2 
        ON dh2.haulage = haulages.id AND dh2.driver_number = 2 
    LEFT JOIN users du2 
        ON du2.id = dh2.driver 
    ORDER BY 
        haulages.status,
        haulages.start_time,
        haulages.end_time, 
        routes.name 
    ;
$$ LANGUAGE SQL
;

CREATE FUNCTION getDriverHaulages(driver_id INT)
RETURNS TABLE (
    id int, 
    Статус VARCHAR, 
    Начато TIMESTAMP, 
    Завершено TIMESTAMP, 
    Откуда VARCHAR, 
    Куда VARCHAR, 
    База MONEY,
    Бонус MONEY,
    Сумма MONEY
) AS $$
    WITH temp AS ( 
        SELECT 
            haulages.id AS id, 
            haulages.status AS Статус, 
            haulages.start_time AS Начато, 
            haulages.end_time AS Завершено, 
            routes.start_point AS Откуда, 
            routes.end_point AS Куда, 
            routes.drv_fee_base AS База, 
            drv_haul.driver_bonus AS Бонус, 
            routes.drv_fee_base + drv_haul.driver_bonus AS Сумма 
        FROM haulages 
        JOIN routes 
            ON haulages.route = routes.id 
        JOIN drv_haul 
            ON drv_haul.haulage = haulages.id 
        JOIN users 
            ON users.id = drv_haul.driver 
        WHERE 
            users.id = driver_id 
        ORDER BY 
            haulages.status,
            haulages.start_time,
            haulages.end_time, 
            routes.name 
    ) (
    SELECT 
        id, 
        CASE 
            WHEN Статус = 0 THEN 'В работе' 
            WHEN Статус = 1 THEN 'Завершено' 
            WHEN Статус = 2 THEN 'Отменено' 
            ELSE 'Неизвестно' 
        END Статус, 
        Начато, Завершено, 
        Откуда,  Куда, 
        База, Бонус, 
        Сумма 
    FROM temp 
    ) UNION (
    SELECT 
        NULL AS id,
        'Итого' AS Статус, 
        NULL AS Начато, NULL AS Завершено, 
        NULL AS Откуда, NULL AS Куда, 
        SUM(База) AS База, SUM(Бонус) AS Бонус, 
        SUM(Сумма) AS Сумма 
    FROM temp 
    WHERE Статус != 0 
    ) 
    ;
$$ LANGUAGE SQL
;

CREATE FUNCTION addRoute(
    name VARCHAR, 
    start_point VARCHAR, end_point VARCHAR,
    len INT, details VARCHAR,
    client_price MONEY, drv_fee_base MONEY
) RETURNS void
AS $$
    INSERT INTO routes (
        name, active, start_point, end_point, len, 
        details, client_price, drv_fee_base
    ) VALUES (
        name, true, start_point, end_point, len, 
        details, client_price, drv_fee_base
    )
    ;
$$ LANGUAGE SQL
;

CREATE FUNCTION activateRoute(route_id INT, route_active BOOLEAN) 
RETURNS void
AS $$
    UPDATE routes
    SET active = route_active
    WHERE id = route_id
    ;
$$ LANGUAGE SQL
;

CREATE FUNCTION addHaulage(
    route_id INT,
    driver_1 INT, driver_1_bonus INT,
    driver_2 INT, driver_2_bonus INT
) RETURNS void
AS $$
    WITH new_haulage AS (
        INSERT INTO haulages (
            status, route, start_time, end_time
        ) VALUES (
            0, route_id, NOW(), NULL
        ) RETURNING id
    ) INSERT INTO drv_haul (
        haulage, driver, driver_number, driver_bonus
    ) (
        SELECT id, driver_1, 1, driver_1_bonus
        FROM new_haulage
        UNION
        SELECT id, driver_2, 2, driver_2_bonus
        FROM new_haulage
    )
    ;
$$ LANGUAGE SQL
;

CREATE FUNCTION addHaulage(
    route_id INT,
    driver INT, driver_bonus INT
) RETURNS void
AS $$
    WITH new_haulage AS (
        INSERT INTO haulages (
            status, route, start_time, end_time
        ) VALUES (
            0, route_id, NOW(), NULL
        ) RETURNING id
    ) INSERT INTO drv_haul (
        haulage, driver, driver_number, driver_bonus
    ) (
        SELECT id, driver, 1, driver_bonus
        FROM new_haulage
    )
    ;
$$ LANGUAGE SQL
;

CREATE FUNCTION successHaulage(haulage_id INT)
RETURNS void
AS $$
    UPDATE haulages 
    SET 
        status = 1, 
        end_time = now() 
    WHERE id = haulage_id 
    ;
$$ LANGUAGE SQL
;

CREATE FUNCTION cancelHaulage(haulage_id INT)
RETURNS void
AS $$
    UPDATE haulages 
    SET status = 2 
    WHERE id = haulage_id 
    ;
$$ LANGUAGE SQL
;

CREATE FUNCTION reopenHaulage(haulage_id INT)
RETURNS void
AS $$
    UPDATE haulages 
    SET 
        status = 0, 
        end_time = NULL
    WHERE id = haulage_id 
    ;
$$ LANGUAGE SQL
;

