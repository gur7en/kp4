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
    void openDriverDetailTab(const QString userID);
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


class GeneralizedListTab : public QWidget
{
    Q_OBJECT

public:
    GeneralizedListTab(DataBase *db);

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


class DriversTab : public GeneralizedListTab
{
    Q_OBJECT

public:
    DriversTab(DataBase *db);

public slots:
    void resetQueryModel();
    void showTableContextMenu(QPoint position);
    void detailSelectedDriver();


signals:
    void requestDriverDetail(const QString userID);

private:
    QAction *showTranspMenuAction;

 };


class LogistsTab : public GeneralizedListTab
{
    Q_OBJECT

public:
    LogistsTab(DataBase *db);

public slots:
    void resetQueryModel();

};


class AccountersTab : public GeneralizedListTab
{
    Q_OBJECT

public:
    AccountersTab(DataBase *db);

public slots:
    void resetQueryModel();

};


class RoutesTab : public GeneralizedListTab
{
    Q_OBJECT

public:
    RoutesTab(DataBase *db);

public slots:
    void resetQueryModel();
    void showTableContextMenu(QPoint position);

signals:
    void requestAddRoute();
    void requestEditRoute();

private:
    QAction *addRouteMenuAction;
    QAction *editRouteMenuAction;
};


class TransportationsTab : public GeneralizedListTab
{
    Q_OBJECT

public:
    TransportationsTab(DataBase *db);

public slots:
    void resetQueryModel();

signals:
    void requestAddTransportation();
    // void requestEditTransportation();

private:
    QAction *addTranspMenuAction;
};


class DriverDetailTab : public QWidget
{
    Q_OBJECT

public:
    DriverDetailTab(DataBase *db, const QString &userID = "", bool closable = false);

public slots:
    void showTableContextMenu(QPoint position);
    void resetQueryModel();
    void close();

private:
    DataBase *db;
    QString userID;
    QComboBox *selectPeriodCombo;
    QDateEdit *fromDate;
    QDateEdit *toDate;
    QPushButton *confirmPeriodButton;
    QLineEdit *userEditRO;
    QLineEdit *salaryForPeriodRO;
    QTableView *table;
    QSqlQueryModel *tableModel;
    QMenu *tableContextMenu;
    QAction *updateTableAction;
    QPushButton *closeButton;
};


class AddRouteTab : public QWidget
{
    Q_OBJECT

public:
    AddRouteTab(DataBase *db);

public slots:
    void addRoute();
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
    void editRoute();
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
    void addTransportation();
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
