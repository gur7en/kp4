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

    QString str_query;
    str_query = "SELECT * FROM getLoginData(:login, :hash);";

    QSqlQuery query(db);
    query.prepare(str_query);
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
    str_query = "SELECT * FROM checkPassword(:id, :hash);";

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


QSqlQuery DataBase::users(UserRole::Code role_code)
{
    UserRole role(role_code);
    QString str_query;
    if(role == UserRole::Driver) {
        str_query = "SELECT * FROM getDrivers();";
    } else {
        str_query = "SELECT * FROM getUsers(:role);";
    }

    QSqlQuery query(db);
    query.prepare(str_query);
    query.bindValue(":role", role.toString());
    query.exec();

    return query;
}


QSqlQuery DataBase::usersList(UserRole::Code role_code, bool with_empty)
{
    UserRole role(role_code);
    QString str_query;

    if(with_empty) {
        str_query = "SELECT * FROM getUsersShortWithEmpty(:role);";
    } else {
        str_query = "SELECT * FROM getUsersShort(:role);";
    }

    QSqlQuery query(db);
    query.prepare(str_query);
    query.bindValue(":role", role.toString());
    query.exec();

    return query;
}


QSqlQuery DataBase::routes()
{
    QString str_query;
    str_query += "SELECT * FROM getActiveRoutes();";

    QSqlQuery query(db);
    query.prepare(str_query);
    query.exec();

    return query;
}


QSqlQuery DataBase::routesList()
{
    QString str_query;
    str_query += "SELECT * FROM getActiveRoutesShort();";

    QSqlQuery query(db);
    query.prepare(str_query);
    query.exec();

    return query;
}


QSqlQuery DataBase::routesQueryAll()
{
    QString str_query;
    str_query += "SELECT * FROM getAllRoutes();";

    QSqlQuery query(db);
    query.prepare(str_query);
    query.exec();

    return query;
}


bool DataBase::routeInfo(int id,
                         QString &out_name,
                         QString &out_start, QString &out_end,
                         int &out_length, QString &out_details,
                         int &out_client_price, int &out_driver_fee_base)
{
    QString str_query;
    str_query += "SELECT * FROM getRouteInfo(:id);";

    QSqlQuery query(db);
    query.prepare(str_query);
    query.bindValue(":id", id);
    query.exec();

    query.next();
    if(!query.value(0).isNull()) {
        out_name = query.value("name").toString();
        out_start = query.value("start_point").toString();
        out_end = query.value("end_point").toString();
        out_length = query.value("len").toString().toDouble();
        out_details = query.value("details").toString();
        out_client_price = query.value("client_price").toString().toDouble();
        out_driver_fee_base = query.value("drv_fee_base").toString().toDouble();
        return true;
    } else {
        return false;
    }
}


QSqlQuery DataBase::addRoute(const QString &name,
                             const QString &start, const QString &end,
                             int length, const QString &details,
                             int client_price, int driver_fee_base)
{
    QString str_query;
    str_query += "INSERT INTO routes ("
                 "    name, active, start_point, end_point, "
                 "    len, details, client_price, drv_fee_base "
                 "    ) "
                 "VALUES ("
                 "    :name, true, :start, :end, "
                 "    :len, :details, :clprice, :drvfee"
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


QSqlQuery DataBase::activateRoute(int id)
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


QSqlQuery DataBase::deactivateRoute(int id)
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


QSqlQuery DataBase::haulages()
{
    QString str_query;
    str_query += "SELECT * FROM getHaulages();";

    QSqlQuery query(db);
    query.prepare(str_query);
    query.exec();

    return query;
}


QSqlQuery DataBase::addHaulage(int route,
                               int first_driver, int first_driver_bonus,
                               int second_driver, int second_driver_bonus)
{
    QString str_query;
    str_query = "INSERT INTO haulages "
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

    int new_haulage = 0;
    if(success) {
        new_haulage = query.value("id").toInt();
    } else {
        return query;
    }

    str_query = "INSERT INTO drv_haul "
                "    (haulage, driver, driver_number, driver_bonus) "
                "VALUES "
                "    (:new_haulage, :first_driver, 1, :first_driver_bonus) "
        ;

    if(second_driver != 0 && first_driver != second_driver) {
        str_query += "  , (:new_haulage, :second_driver, 2, :second_driver_bonus) "
                     ";"
            ;
    }

    query.prepare(str_query);
    query.bindValue(":new_haulage", new_haulage);
    query.bindValue(":first_driver", first_driver);
    query.bindValue(":second_driver", second_driver);
    query.bindValue(":first_driver_bonus", first_driver_bonus);
    query.bindValue(":second_driver_bonus", second_driver_bonus);
    query.exec();

    return query;
}


QSqlQuery DataBase::driverHaulages(int id)
{
    QString str_query;
    str_query += "SELECT * FROM getDriverHaulages(:id);";

    QSqlQuery query(db);
    query.prepare(str_query);

    if(id == 0) {
        id = currentUserID;
    }

    query.bindValue(":id", id);
    query.exec();

    return query;
}


QSqlQuery DataBase::successHaulage(int id)
{
    QString str_query;
    str_query += "UPDATE "
                 "    haulages "
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


QSqlQuery DataBase::cancelHaulage(int id)
{
    QString str_query;
    str_query += "UPDATE "
                 "    haulages "
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


QSqlQuery DataBase::reopenHaulage(int id)
{
    QString str_query;
    str_query += "UPDATE "
                 "    haulages "
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


