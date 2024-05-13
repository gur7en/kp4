#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QMenu>
#include <QSqlQueryModel>

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

#include "database.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent, DataBase *db);
    ~MainWindow();

public slots:
    void openLoginTab();
    void openProfileTab();
    void openTabsForLogist();
    void openTabsForDriver();
    void openTabsForAccounter();
    void openDriverDetailTab();
    void openAddRouteTab();
    void openEditRouteTab();
    void openAddTransportationTab();

private:
    QWidget *central;
    QHBoxLayout *layout;
    QTabWidget *tabWidget;
    DataBase *db;

    void addTab(QWidget *tab, const QString &tabname);
    void resetTabs();
};


class UserListTab : public QWidget
{
    Q_OBJECT

public:
    UserListTab(DataBase *db);

public slots:
    virtual void showTableContextMenu(QPoint position);
    virtual void resetQueryModel();

signals:
    void requestUpdateTable();

protected:
    DataBase *db;
    QTableView *table;
    QSqlQueryModel *tableModel;
    QMenu *tableContextMenu;
    QAction *updateTableAction;

    virtual bool actionTableContextMenu(QAction *selected);

};


class LoginTab : public QWidget
{
    Q_OBJECT

public:
    LoginTab(DataBase *db);

signals:
    void userLoginAsLogist();
    void userLoginAsDriver();
    void userLoginAsAccounter();

private:
    DataBase *db;
    QLineEdit *loginEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;

private slots:
    void loginPressed();
};


class ProfileTab : public QWidget
{
    Q_OBJECT

public:
    ProfileTab(DataBase *db);

signals:
    void userLogout();

private:
    DataBase *db;
    QLineEdit *userEditRO;
    QLineEdit *userRoleEditRO;
    QPushButton *logoutButton;
    QLineEdit *oldPasswordEdit;
    QLineEdit *newPasswordEdit;
    QLineEdit *repeatPasswordEdit;
    QPushButton *changePasswordButton;

private slots:
    void logoutPressed();
};


class DriversTab : public UserListTab
{
    Q_OBJECT

public:
    DriversTab(DataBase *db);

public slots:
    void showTableContextMenu(QPoint pos);
    void resetQueryModel();

signals:
    void requestDriverDetail(const QString &username);

private:
    QAction *showTranspMenuAction;

    virtual bool actionTableContextMenu(QAction *selected);

};


class LogistsTab : public UserListTab
{
    Q_OBJECT

public:
    LogistsTab(DataBase *db);

public slots:
    void resetQueryModel();

};


class AccountersTab : public UserListTab
{
    Q_OBJECT

public:
    AccountersTab(DataBase *db);

public slots:
    void resetQueryModel();

};


class RoutesTab : public QWidget
{
    Q_OBJECT

public:
    RoutesTab(DataBase *db);

public slots:
    void showTableContextMenu(QPoint pos);

signals:
    void requestAddRoute();
    void requestEditRoute();

private:
    DataBase *db;
    QTableView *table;
    QMenu *tableContextMenu;
    QAction *addRouteMenuAction;
    QAction *editRouteMenuAction;
    QItemSelectionModel *tableSelectionModel;
};


class TransportationsTab : public QWidget
{
    Q_OBJECT

public:
    TransportationsTab(DataBase *db);

public slots:
    void showTableContextMenu(QPoint pos);

signals:
    void requestAddTransportation();
    void requestEditTransportation();

private:
    DataBase *db;
    QTableView *table;
    QMenu *tableContextMenu;
    QAction *addTranspMenuAction;
};


class DriverDetailTab : public QWidget
{
    Q_OBJECT

public:
    DriverDetailTab(DataBase *db);

private:
    DataBase *db;
    QComboBox *selectPeriodCombo;
    QDateEdit *fromDate;
    QDateEdit *toDate;
    QPushButton *confirmPeriodButton;
    QLineEdit *userEditRO;
    QLineEdit *salaryForPeriodRO;
    QTableView *table;
};


class AddRouteTab : public QWidget
{
    Q_OBJECT

public:
    AddRouteTab(DataBase *db);

public slots:
    void close();

private:
    DataBase *db;
    QLineEdit *routeNameEdit;
    QLineEdit *fromEdit;
    QLineEdit *toEdit;
    QDoubleSpinBox *lengthSpin;
    QSpinBox *priceSpin;
    QLineEdit *descriptionEdit;
    QPushButton *addRouteButton;
    QPushButton *cancelButton;
};


class EditRouteTab : public QWidget
{
    Q_OBJECT

public:
    EditRouteTab(DataBase *db);

public slots:
    void close();

private:
    DataBase *db;
    QLineEdit *routeNameEdit;
    QLineEdit *fromEdit;
    QLineEdit *toEdit;
    QDoubleSpinBox *lengthSpin;
    QSpinBox *priceSpin;
    QLineEdit *descriptionEdit;
    QPushButton *confirmButton;
    QPushButton *cancelButton;
};


class AddTransportationTab : public QWidget
{
    Q_OBJECT

public:
    AddTransportationTab(DataBase *db);

public slots:
    void close();

private:
    DataBase *db;
    QComboBox *firstDriverCombo;
    QSpinBox *firstDriverBonusSpin;
    QComboBox *secondDriverCombo;
    QSpinBox *secondDriverBonusSpin;
    QComboBox *routeCombo;
    QPushButton *addTranspButton;
    QPushButton *cancelButton;
};

#endif // MAINWINDOW_H
