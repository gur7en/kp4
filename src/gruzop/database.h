#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>

class UserRole
{
public:
    enum Code {
        Unknown,
        Driver,
        Logist,
        Accounter
    };

    Code operator = (const UserRole::Code code)
    { return (roleCode = code); }

    bool operator == (const UserRole::Code code) const
    { return (roleCode == code); }

    operator Code() const
    { return roleCode; }

private:
    Code roleCode;
};


class DataBase : public QObject
{
    Q_OBJECT
public:
    explicit DataBase(QObject *parent = nullptr);
    bool reconnect(const QString user, const QString password);
    bool login(const QString &username, const QString &password);
    void logout();
    UserRole::Code getUserRole();
    QString getUserFullName();
    QString getUserShortName();

private:
    struct CurrentUser {
        int id;
        UserRole role;
        QString login;
        QString surname;
        QString name;
        QString patronim;
    };

    QSqlDatabase db;
    CurrentUser user;

signals:
};


#endif // DATABASE_H
