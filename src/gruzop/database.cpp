#include "database.h"


QVariant QueryModel::data(const QModelIndex &item, int role) const
{
    QVariant result;

    switch(role) {
    case Qt::UserRole:
        result = query().value("id").toInt();
        break;
    default:
        result = QSqlQueryModel::data(item, role);
        break;
    }

    return result;
}


void QueryModel::setQuery(QSqlQuery &&query)
{
    QSqlQueryModel::setQuery(std::move(query));
    removeColumn(record().indexOf("id"));
}


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

    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(password.toUtf8());

    QSqlQuery query(db);
    query.prepare("SELECT id, role FROM users "
                  "WHERE login = :login AND hash = :hash "
                  "AND fired = false ;"
                  );
    query.bindValue(":login", username);
    query.bindValue(":hash", QString(hash.result().toHex()));

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


QString DataBase::userFullName(int id)
{
    if(id == 0) {
        id = currentUserID;
    }

    QSqlQuery query(db);
    query.prepare("SELECT getFullName(:id) ;");
    query.bindValue(":id", id);

    bool success = query.exec();

    if(success) {
        query.next();
        success = !query.value(0).isNull();
    }

    if(success) {
        return query.value(0).toString();
    }

    return "";
}


QString DataBase::userShortName(int id)
{
    if(id == 0) {
        id = currentUserID;
    }

    QSqlQuery query(db);
    query.prepare("SELECT getShortName(:id) ;");
    query.bindValue(":id", id);

    bool success = query.exec();

    if(success) {
        query.next();
        success = !query.value(0).isNull();
    }

    if(success) {
        return query.value(0).toString();
    }

    return "";
}


bool DataBase::checkPasswordCurrentUser(QString &password)
{
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(password.toUtf8());

    QString str_query;
    str_query += "SELECT EXISTS ( "
                 "    SELECT id "
                 "    FROM users "
                 "    WHERE id = :id AND hash = :hash "
                 ")"
                 ";"
        ;

    QSqlQuery query(db);
    query.prepare(str_query);
    query.bindValue(":id", currentUserID);
    query.bindValue(":hash", QString(hash.result().toHex()));

    bool success = query.exec();

    if(success) {
        success = !query.lastError().isValid();
    }

    if(success) {
        query.next();
        success = (query.value(0).toString() == "true");
    }

    return success;
}


bool DataBase::changePasswordCurrentUser(QString &new_password)
{
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(new_password.toUtf8());

    QString str_query;
    str_query += "UPDATE users "
                 "SET hash = :hash "
                 "WHERE id = :id "
                 ";"
        ;

    QSqlQuery query(db);
    query.prepare(str_query);
    query.bindValue(":id", currentUserID);
    query.bindValue(":hash", QString(hash.result().toHex()));

    bool success = query.exec();

    if(success) {
        success = !query.lastError().isValid();
    }

    return success;
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


QSqlQuery DataBase::usersListQuery(UserRole::Code role_code, bool with_empty)
{
    UserRole role(role_code);
    QString str_query;

    if(with_empty) {
        str_query += "SELECT * "
                     "FROM ( "
                     "SELECT"
                     "    0 AS id, "
                     "    '<Никто не выбран>' AS name "
                     ") UNION ( "
            ;
    }

    str_query += "SELECT "
                 "    id, "
                 "    getFullName(id) AS name "
                 "FROM "
                 "    users "
                 "WHERE "
                 "    role = :role "
        ;

    if(with_empty) {
        str_query += ") "
            ;
    }

    str_query += ";"
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
                 "WHERE active = true "
                 "ORDER BY "
                 "    active DESC,"
                 "    start_point,"
                 "    end_point, "
                 "    name "
                 ";"
        ;

    QSqlQuery query(db);
    query.prepare(str_query);
    query.exec();

    return query;
}


QSqlQuery DataBase::routesListQuery()
{
    QString str_query;
    str_query += "SELECT "
                 "    id, "
                 "    CONCAT( "
                 "        name, "
                 "        ' (', "
                 "        start_point, "
                 "        ' -> ', "
                 "        end_point, "
                 "        ', ', "
                 "        len, ' км', "
                 "        ')' "
                 "    ) "
                 "FROM routes "
                 "WHERE active = true "
                 "ORDER BY "
                 "    name, "
                 "    start_point, "
                 "    end_point "
                 ";"
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
                 "FROM routes "
                 "ORDER BY "
                 "    active DESC,"
                 "    start_point,"
                 "    end_point, "
                 "    name "
                 ";"
        ;

    QSqlQuery query(db);
    query.prepare(str_query);
    query.exec();

    return query;
}


QSqlQuery DataBase::routeByIdQuery(int id)
{
    QString str_query;
    str_query += "SELECT "
                 "    id, "
                 "    name, "
                 "    start_point, "
                 "    end_point, "
                 "    len, "
                 "    client_price::numeric AS client_price, "
                 "    drv_fee_base::numeric AS drv_fee_base, "
                 "    details "
                 "FROM routes "
                 "WHERE id = :id "
                 ";"
        ;

    QSqlQuery query(db);
    query.prepare(str_query);
    query.bindValue(":id", id);
    query.exec();

    return query;
}


void DataBase::parseRoute(QSqlQuery &query,
                          QString &out_name,
                          QString &out_start, QString &out_end,
                          int &out_length, QString &out_details,
                          int &out_client_price, int &out_driver_fee_base)
{
    query.next();
    if(!query.value(0).isNull()) {
        out_name = query.value("name").toString();
        out_start = query.value("start_point").toString();
        out_end = query.value("end_point").toString();
        out_length = query.value("len").toString().toDouble();
        out_details = query.value("details").toString();
        out_client_price = query.value("client_price").toString().toDouble();
        out_driver_fee_base = query.value("drv_fee_base").toString().toDouble();
    }
}


QSqlQuery DataBase::addRouteQuery(const QString &name,
                                  const QString &start, const QString &end,
                                  int length, const QString &details,
                                  int client_price, int driver_fee_base)
{
    QString str_query;
    str_query += "INSERT INTO routes "
                 "    (name, active, start_point, end_point, "
                 "     len, details, client_price, drv_fee_base "
                 "    ) "
                 "VALUES "
                 "    (:name, true, :start, :end, "
                 "     :len, :details, :clprice, :drvfee"
                 "    ) "
                 ";"
        ;

    QSqlQuery query(db);
    query.prepare(str_query);
    query.bindValue(":name", name);
    query.bindValue(":start", start);
    query.bindValue(":end", end);
    query.bindValue(":len", length);
    query.bindValue(":details", details);
    query.bindValue(":clprice", client_price);
    query.bindValue(":drvfee", driver_fee_base);
    query.exec();

    return query;
}


QSqlQuery DataBase::activateRouteQuery(int id)
{
    QString str_query;
    str_query += "UPDATE "
                 "    routes "
                 "SET "
                 "    active = true "
                 "WHERE "
                 "    id = :id "
                 ";"
        ;

    QSqlQuery query(db);
    query.prepare(str_query);
    query.bindValue(":id", id);
    query.exec();

    return query;
}


QSqlQuery DataBase::deactivateRouteQuery(int id)
{
    QString str_query;
    str_query += "UPDATE "
                 "    routes "
                 "SET "
                 "    active = false "
                 "WHERE "
                 "    id = :id "
                 ";"
        ;

    QSqlQuery query(db);
    query.prepare(str_query);
    query.bindValue(":id", id);
    query.exec();

    return query;
}


QSqlQuery DataBase::transpQuery()
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


QSqlQuery DataBase::addTranspQuery(int route,
                                   int first_driver, int first_driver_bonus,
                                   int second_driver, int second_driver_bonus)
{
    QString str_query;
    str_query = "INSERT INTO transportations "
                "    (status, route, start_time, end_time) "
                "VALUES "
                "    (0, :route, Now(), NULL) "
                "RETURNING id "
                ";"
        ;

    QSqlQuery query(db);
    query.prepare(str_query);
    query.bindValue(":route", route);

    bool success;
    success = query.exec();

    if(success) {
        query.next();
        success = !query.value(0).isNull();
    }

    int new_transp = 0;
    if(success) {
        new_transp = query.value("id").toInt();
    } else {
        return query;
    }

    str_query = "INSERT INTO drv_transp "
                "    (transp, driver, driver_number, driver_bonus) "
                "VALUES "
                "    (:new_transp, :first_driver, 1, :first_driver_bonus) "
        ;

    if(second_driver != 0 && first_driver != second_driver) {
        str_query += "  , (:new_transp, :second_driver, 2, :second_driver_bonus) "
                     ";"
            ;
    }

    query.prepare(str_query);
    query.bindValue(":new_transp", new_transp);
    query.bindValue(":first_driver", first_driver);
    query.bindValue(":second_driver", second_driver);
    query.bindValue(":first_driver_bonus", first_driver_bonus);
    query.bindValue(":second_driver_bonus", second_driver_bonus);
    query.exec();

    return query;
}


QSqlQuery DataBase::driverTranspQuery(int id)
{
    QString str_query;
    str_query += "WITH temp AS ( "
                 "SELECT "
                 "    tr.id AS id, "
                 "    tr.status AS Статус, "
                 "    tr.start_time AS Начато, "
                 "    tr.end_time AS Завершено, "
                 "    routes.start_point AS Откуда, "
                 "    routes.end_point AS Куда, "
                 "    routes.drv_fee_base AS База, "
                 "    drv_transp.driver_bonus AS Бонус, "
                 "    routes.drv_fee_base + drv_transp.driver_bonus AS Сумма "
                 "FROM transportations tr "
                 "JOIN routes "
                 "    ON tr.route = routes.id "
                 "JOIN drv_transp "
                 "    ON drv_transp.transp = tr.id "
                 "JOIN users "
                 "    ON users.id = drv_transp.driver "
                 "WHERE "
                 "    users.id = :id "
                 "ORDER BY "
                 "    tr.status,"
                 "    tr.start_time,"
                 "    tr.end_time, "
                 "    routes.name "
                 ") ("
                 "SELECT "
                 "    id, "
                 "    CASE "
                 "        WHEN Статус = 0 THEN "
                 "            'В работе' "
                 "        WHEN Статус = 1 THEN "
                 "            'Завершено' "
                 "        WHEN Статус = 2 THEN "
                 "            'Отменено' "
                 "        ELSE "
                 "            'Неизвестно' "
                 "    END Статус, "
                 "    Начато, Завершено, "
                 "    Откуда,  Куда, "
                 "    База, Бонус, "
                 "    Сумма "
                 "FROM temp "
                 ") UNION ("
                 "SELECT "
                 "    NULL AS id,"
                 "    'Итого' AS Статус, "
                 "    NULL AS Начато, NULL AS Завершено, "
                 "    NULL AS Откуда, NULL AS Куда, "
                 "    SUM(База) AS База, SUM(Бонус) AS Бонус, "
                 "    SUM(Сумма) AS Сумма "
                 "FROM temp "
                 "WHERE Статус != 0 "
                 ") "
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


QSqlQuery DataBase::successTranspQuery(int id)
{
    QString str_query;
    str_query += "UPDATE "
                 "    transportations "
                 "SET "
                 "    status = 1, "
                 "    end_time = now() "
                 "WHERE "
                 "    id = :id "
                 ";"
        ;

    QSqlQuery query(db);
    query.prepare(str_query);
    query.bindValue(":id", id);
    query.exec();

    return query;
}


QSqlQuery DataBase::cancelTranspQuery(int id)
{
    QString str_query;
    str_query += "UPDATE "
                 "    transportations "
                 "SET "
                 "    status = 2,"
                 "    end_time = now() "
                 "WHERE "
                 "    id = :id "
                 ";"
        ;

    QSqlQuery query(db);
    query.prepare(str_query);
    query.bindValue(":id", id);
    query.exec();

    return query;
}


QSqlQuery DataBase::reopenTranspQuery(int id)
{
    QString str_query;
    str_query += "UPDATE "
                 "    transportations "
                 "SET "
                 "    status = 0,"
                 "    end_time = NULL "
                 "WHERE "
                 "    id = :id "
                 ";"
        ;

    QSqlQuery query(db);
    query.prepare(str_query);
    query.bindValue(":id", id);
    query.exec();

    return query;
}


