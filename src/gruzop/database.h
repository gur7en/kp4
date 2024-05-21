#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>

class UserRole
{
public:
    enum Code {
        Unlogin,
        Unknown,
        Driver,
        Logist,
        Accounter
    };

    UserRole(Code code)
    { roleCode = code; }

    Code operator = (const UserRole::Code code)
    { return (roleCode = code); }

    bool operator == (const UserRole::Code code) const
    { return (roleCode == code); }

    operator Code() const
    { return roleCode; }

    QString toString() const
    { return strings[roleCode]; }

private:
    Code roleCode;
    const QString strings[5] = {
        "unlogin",
        "unknown",
        "driver",
        "logist",
        "accounter"
    };
};


class DataBase : public QObject
{
    Q_OBJECT
public:
    explicit DataBase(QObject *parent = nullptr);
    bool reconnect(const QString user, const QString password);
    void disconnect();
    UserRole::Code login(const QString &username, const QString &password);
    void logout();
    int userID();
    UserRole::Code userRole();
    QString userFullName(int id = 0);
    QString userShortName(int id = 0);
    QSqlQuery usersQuery(UserRole::Code role_code);
    QSqlQuery routesQuery();
    QSqlQuery addRouteQuery(const QString &name,
                            const QString &start, const QString &end,
                            int length, const QString &details,
                            int client_price, int driver_fee_base);
    QSqlQuery routesQueryAll();
    QSqlQuery transpQuery();
    QSqlQuery driverTranspQuery(int id = 0);
    QSqlQuery addTranspQuery();
    QSqlQuery cancelTranspQuery(int id);
    QSqlQuery successTranspQuery(int id);
    QSqlQuery reopenTranspQuery(int id);

private:
    QSqlDatabase db;
    int currentUserID;
    UserRole::Code currentUserRole;

signals:
};


#endif // DATABASE_H
