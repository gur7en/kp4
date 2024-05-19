#include "database.h"

DataBase::DataBase(QObject *parent)
    : QObject{parent}
{
    db = QSqlDatabase::addDatabase("QPSQL");
    db.setDatabaseName("kp4db");
    db.setHostName("127.0.0.1");
}


bool DataBase::reconnect(const QString user, const QString password)
{
    db.close();
    return db.open(user, password);
}


void DataBase::disconnect()
{
    db.close();
}


UserRole::Code DataBase::login(const QString &username, const QString &password)
{
    UserRole::Code result = UserRole::Unlogin;

    QSqlQuery query(db);
    query.prepare("SELECT id, role FROM users "
                  "WHERE login = :login AND hash = :hash "
                  "AND fired = false ;"
                  );
    query.bindValue(":login", username);
    query.bindValue(":hash", password);

    bool success = query.exec();

    if(success) {
        query.next();
        success = !query.value(0).isNull();
    }

    if(success) {
        currentUserID = query.value("id").toInt();
        QString role_string;
        role_string = query.value("role").toString();
        if(role_string == "driver") {
            result = UserRole::Driver;
        } else if(role_string == "logist") {
            result = UserRole::Logist;
        } else if(role_string == "accounter") {
            result = UserRole::Accounter;
        } else {
            result = UserRole::Unknown;
        }
    }
    currentUserRole = result;
    return result;
}


void DataBase::logout()
{
    currentUserID = 0;
}


int DataBase::userID()
{
    return currentUserID;
}


UserRole::Code DataBase::userRole()
{
    return currentUserRole;
}


QString DataBase::userFullName()
{
    QSqlQuery query(db);
    query.prepare("SELECT getFullName(:id) ;");
    query.bindValue(":id", currentUserID);

    bool success = query.exec();

    if(success) {
        query.next();
        success = !query.value(0).isNull();
    }

    if(success) {
        return query.value("FullName").toString();
    }

    return "";
}


QString DataBase::userShortName()
{
    QSqlQuery query(db);
    query.prepare("SELECT getShortName(:id) ;");
    query.bindValue(":id", currentUserID);

    bool success = query.exec();

    if(success) {
        query.next();
        success = !query.value(0).isNull();
    }

    if(success) {
        return query.value("FullName").toString();
    }

    return "";
}


QSqlQuery DataBase::usersQuery(UserRole::Code role_code)
{
    UserRole role(role_code);
    QString str_query;
    str_query += "SELECT "
                 "    id, "
                 "    surname AS Фамилия, "
                 "    name AS Имя, "
                 "    patronim AS Отчество, "
                 "    phone AS Телефон, "
                 "    details AS Примечание "
        ;
    if(role == UserRole::Driver) {
        str_query += ", experience AS Стаж ";
    }

    str_query += "FROM users "
                 "LEFT JOIN drv_exp ON users.id = drv_exp.driver "
                 "WHERE role = :role "
                 "AND fired = false ;"
        ;

    QSqlQuery query(db);
    query.prepare(str_query);
    query.bindValue(":role", role.toString());
    query.exec();

    return query;
}


QSqlQuery DataBase::routesQuery()
{
    QString str_query;
    str_query += "SELECT "
                 "    id, "
                 "    name AS Название, "
                 "    start_point AS Откуда, "
                 "    end_point AS Куда, "
                 "    len AS \"Длина (км)\", "
                 "    client_price AS Цена, "
                 "    details AS Примечание "
                 "FROM routes "
                 "WHERE active = true ;"
        ;

    QSqlQuery query(db);
    query.prepare(str_query);
    query.exec();

    return query;
}


QSqlQuery DataBase::routesQueryAll()
{
    QString str_query;
    str_query += "SELECT "
                 "    id, "
                 "    CASE "
                 "        WHEN active = true THEN "
                 "            'Открыт' "
                 "        WHEN active = false THEN "
                 "            'Закрыт' "
                 "        ELSE "
                 "            'Неизвестно' "
                 "    END Статус, "
                 "    name AS Название, "
                 "    start_point AS Откуда, "
                 "    end_point AS Куда, "
                 "    len AS \"Длина (км)\", "
                 "    client_price AS Цена, "
                 "    details AS Примечание "
                 "FROM routes ;"
        ;

    QSqlQuery query(db);
    query.prepare(str_query);
    query.exec();

    return query;
}


QSqlQuery DataBase::transportationsQuery()
{
    QString str_query;
    str_query += "SELECT "
                 "    tr.id AS id, "
                 "    CASE "
                 "        WHEN tr.status = 0 THEN "
                 "            'В работе' "
                 "        WHEN tr.status = 1 THEN "
                 "            'Завершено' "
                 "        WHEN tr.status = 2 THEN "
                 "            'Отменено' "
                 "        ELSE "
                 "            'Неизвестно' "
                 "        END Статус, "
                 "    tr.start_time AS Начато, "
                 "    tr.end_time AS Завершено, "
                 "    routes.name AS Маршрут, "
                 "    routes.start_point AS Откуда, "
                 "    routes.end_point AS Куда, "
                 "    getShortName(du1.id) AS \"Водитель 1\", "
                 "    getShortName(du2.id) AS \"Водитель 2\" "
                 "FROM transportations tr "
                 "JOIN routes "
                 "    ON tr.route = routes.id "
                 "JOIN drv_transp dt1 "
                 "    ON dt1.transp = tr.id "
                 "    AND dt1.driver_number = 1 "
                 "JOIN users du1 "
                 "    ON du1.id = dt1.driver "
                 "LEFT JOIN drv_transp dt2 "
                 "    ON dt2.transp = tr.id "
                 "    AND dt2.driver_number = 2 "
                 "LEFT JOIN users du2 "
                 "    ON du2.id = dt2.driver "
                 "ORDER BY "
                 "    tr.status,"
                 "    tr.start_time,"
                 "    tr.end_time, "
                 "    routes.name "
                 ";"
        ;

    QSqlQuery query(db);
    query.prepare(str_query);
    query.exec();

    return query;
}


QSqlQuery DataBase::driverTranspQuery(int id)
{
    QString str_query;
    str_query += "SELECT "
                 "    tr.id AS id, "
                 "    tr.status AS Статус, "
                 "    tr.start_time AS Начато, "
                 "    tr.end_time AS Завершено, "
                 "    routes.start_point AS Откуда, "
                 "    routes.end_point AS Куда, "
                 "    routes.drv_fee_base + drv_transp.driver_bonus AS Плата "
                 "FROM transportations tr "
                 "JOIN routes "
                 "    ON tr.route = routes.id "
                 "JOIN drv_transp "
                 "    ON drv_transp.transp = tr.id "
                 "JOIN users "
                 "    ON users.id = drv_transp.driver "
                 "WHERE "
                 "    users.id = :id "
                 ";"
        ;

    QSqlQuery query(db);
    query.prepare(str_query);

    if(id == 0) {
        id = currentUserID;
    }

    query.bindValue(":id", id);
    query.exec();

    return query;
}
