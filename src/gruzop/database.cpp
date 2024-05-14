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


bool DataBase::login(const QString &username, const QString &password)
{
    QSqlQuery query(db);
    QString strquery("SELECT id, login, surname, name, patronim, role "
                     "FROM users "
                     "WHERE login = '%1' "
                     "AND hash = '%2' "
                     "AND fired = false "
                     ";"
                     );
    strquery = strquery.arg(username);
    strquery = strquery.arg(password);

    bool success = query.exec(strquery);
    disconnect();

    if(success) {
        query.next();
        success = !query.value(0).isNull();
    }

    if(success) {
        QString role_string;
        role_string = query.value("role").toString();

        user.id = query.value("id").toInt();
        user.login = query.value("login").toString();
        user.surname = query.value("surname").toString();
        user.name = query.value("name").toString();
        user.patronim = query.value("patronim").toString();
        if(role_string == "driver") {
            user.role = UserRole::Driver;
        } else if(role_string == "logist") {
            user.role = UserRole::Logist;
        } else if(role_string == "accounter") {
            user.role = UserRole::Accounter;
        } else {
            user.role = UserRole::Unknown;
        }
    }
    return success;
}


void DataBase::logout()
{
    user = CurrentUser();
}


UserRole::Code DataBase::getUserRole()
{
    return user.role;
}


QString DataBase::getUserShortName()
{
    QString short_name;
    short_name += user.surname;
    short_name += " ";
    short_name += user.name[0];
    short_name += ".";
    if(!user.patronim.isEmpty()) {
        short_name += " ";
        short_name += user.patronim[0];
        short_name += ".";
    }
    return short_name;
}


QString DataBase::getUserFullName()
{
    QString full_name;
    full_name += user.surname;
    full_name += " ";
    full_name += user.name;
    if(!user.patronim.isEmpty()) {
        full_name += " ";
        full_name += user.patronim;
    }
    return full_name;
}


int DataBase::getUserID()
{
    return user.id;
}
