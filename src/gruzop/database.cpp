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
    query.prepare("SELECT getFullName(?) ;");
    query.bindValue(0, id);

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
    query.prepare("SELECT getShortName(?) ;");
    query.bindValue(0, id);

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

    QSqlQuery query(db);
    query.prepare("SELECT * FROM checkPassword(?, ?);");
    query.bindValue(0, currentUserID);
    query.bindValue(1, QString(hash.result().toHex()));

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
    QSqlQuery query(db);
    if(role == UserRole::Driver) {
        query.prepare("SELECT * FROM getDrivers();");
    } else {
        query.prepare("SELECT * FROM getUsers(?);");
    }
    query.bindValue(0, role.toString());
    query.exec();
    return query;
}


QSqlQuery DataBase::usersList(UserRole::Code role_code, bool with_empty)
{
    QSqlQuery query(db);
    if(with_empty) {
        query.prepare("SELECT * FROM getUsersShortWithEmpty(?);");
    } else {
        query.prepare("SELECT * FROM getUsersShort(?);");
    }
    query.bindValue(0, UserRole(role_code).toString());
    query.exec();
    return query;
}


QSqlQuery DataBase::routesList()
{
    QSqlQuery query(db);
    query.prepare("SELECT * FROM getActiveRoutesShort();");
    query.exec();
    return query;
}


QSqlQuery DataBase::routes()
{
    QSqlQuery query(db);
    query.prepare("SELECT * FROM getAllRoutes();");
    query.exec();
    return query;
}


bool DataBase::routeInfo(int id,
                         QString &out_name,
                         QString &out_start, QString &out_end,
                         int &out_length, QString &out_details,
                         int &out_client_price, int &out_driver_fee_base)
{
    QSqlQuery query(db);
    query.prepare("SELECT * FROM getRouteInfo(?);");
    query.bindValue(0, id);
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
    QSqlQuery query(db);
    query.prepare("SELECT * FROM addRoute(?, ?, ?, ?, ?, ?, ?);");
    query.bindValue(0, name);
    query.bindValue(1, start);
    query.bindValue(2, end);
    query.bindValue(3, length);
    query.bindValue(4, details);
    query.bindValue(5, QString::number(client_price));
    query.bindValue(6, QString::number(driver_fee_base));
    query.exec();
    return query;
}


QSqlQuery DataBase::activateRoute(int id, bool activate)
{
    QSqlQuery query(db);
    query.prepare("SELECT * FROM activateRoute(?, ?);");
    query.bindValue(0, id);
    query.bindValue(1, activate);
    query.exec();
    return query;
}


QSqlQuery DataBase::haulages()
{
    QSqlQuery query(db);
    query.prepare("SELECT * FROM getHaulages();");
    query.exec();
    return query;
}


QSqlQuery DataBase::addHaulage(int route,
                               int first_driver, int first_driver_bonus,
                               int second_driver, int second_driver_bonus)
{
    QSqlQuery query(db);
    if(second_driver != 0 && first_driver != second_driver) {
        query.prepare("SELECT * FROM addHaulage(?, ?, ?, ?, ?);");
    } else {
        query.prepare("SELECT * FROM addHaulage(?, ?, ?);");
    }
    query.bindValue(0, route);
    query.bindValue(1, first_driver);
    query.bindValue(2, second_driver);
    query.bindValue(3, first_driver_bonus);
    query.bindValue(4, second_driver_bonus);
    query.exec();
    return query;
}


QSqlQuery DataBase::driverHaulages(int id)
{
    if(id == 0) {
        id = currentUserID;
    }
    QSqlQuery query(db);
    query.prepare("SELECT * FROM getDriverHaulages(?);");
    query.bindValue(0, id);
    query.exec();
    return query;
}


QSqlQuery DataBase::successHaulage(int id)
{
    QSqlQuery query(db);
    query.prepare("SELECT * FROM successHaulage(?);");
    query.bindValue(0, id);
    query.exec();
    return query;
}


QSqlQuery DataBase::cancelHaulage(int id)
{
    QSqlQuery query(db);
    query.prepare("SELECT * FROM cancelHaulage(?);");
    query.bindValue(0, id);
    query.exec();
    return query;
}


QSqlQuery DataBase::reopenHaulage(int id)
{
    QSqlQuery query(db);
    query.prepare("SELECT * FROM reopenHaulage(?);");
    query.bindValue(0, id);
    query.exec();
    return query;
}


