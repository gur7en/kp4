#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QCryptographicHash>

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


class QueryModel : public QSqlQueryModel
{
public:
    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const;
    void setQuery(QSqlQuery &&query);
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
    bool checkPasswordCurrentUser(QString &password);
    bool changePasswordCurrentUser(QString &new_password);
    QSqlQuery users(UserRole::Code role_code);
    QSqlQuery usersList(UserRole::Code role_code, bool with_empty);
    QSqlQuery routes();
    QSqlQuery routesList();
    bool routeInfo(int id,
                   QString &out_name,
                   QString &out_start, QString &out_end,
                   int &out_length, QString &out_details,
                   int &out_client_price, int &out_driver_fee_base);
    QSqlQuery addRoute(const QString &name,
                       const QString &start, const QString &end,
                       int length, const QString &details,
                       int client_price, int driver_fee_base);
    QSqlQuery activateRoute(int id, bool activate);
    QSqlQuery haulages();
    QSqlQuery driverHaulages(int id = 0);
    QSqlQuery addHaulage(int route,
                         int first_driver, int first_driver_bonus,
                         int second_driver, int second_driver_bonus);
    QSqlQuery cancelHaulage(int id);
    QSqlQuery successHaulage(int id);
    QSqlQuery reopenHaulage(int id);

private:
    QSqlDatabase db;
    int currentUserID;
    UserRole::Code currentUserRole;

signals:
};


#endif // DATABASE_H
