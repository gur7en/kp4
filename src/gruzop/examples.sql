\connect kp4db;

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

