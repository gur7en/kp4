#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QMenu>
#include <QDebug>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QIdentityProxyModel>

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


#define PASSWORD_MIN_LEN 5


class GeneralizedListTab : public QWidget
{
    Q_OBJECT

public:
    GeneralizedListTab(DataBase *db);

public slots:
    virtual void showTableContextMenu(QPoint position);
    virtual void resetQueryModel() = 0;

protected:
    DataBase *db;
    QFormLayout *tabLayout;
    QTableView *table;
    QueryModel *tableModel;
    QMenu *tableContextMenu;
    QAction *updateTableAction;

    int selectedID();

};


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
    void openDriverDetailTab(int userID);
    void openAddRouteTab(GeneralizedListTab *requester, int baseRouteID = 0);
    void openAddTransportationTab(GeneralizedListTab *requester);

private:
    QWidget *central;
    QHBoxLayout *layout;
    QTabWidget *tabWidget;
    DataBase *db;

    void addTab(QWidget *tab, const QString &tabname);
    void resetTabs();
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
    void changePasswordPressed();
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
    void requestDriverDetail(int userID);

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
    void addBlankRoute();
    void addBasedOnSelectedRoute();
    void activateRoute();
    void deactivateRoute();

signals:
    void requestAddRoute(RoutesTab *requester);
    void requestAddBasedRoute(RoutesTab *requester, int baseRouteID);

private:
    QAction *addRouteMenuAction;
    QAction *addBasedRouteMenuAction;
    QAction *actRouteMenuAction;
    QAction *deactRouteMenuAction;
};


class TransportationsTab : public GeneralizedListTab
{
    Q_OBJECT

public:
    TransportationsTab(DataBase *db);

public slots:
    void resetQueryModel();
    void addBlankTransportation();
    void successTransportation();
    void cancelTransportation();
    void reopenTransportation();

signals:
    void requestAddTransportation(TransportationsTab *requester);

private:
    QAction *addTranspMenuAction;
    QAction *successTranspMenuAction;
    QAction *cancelTranspMenuAction;
    QAction *reopenTranspMenuAction;
};


class DriverDetailTab : public GeneralizedListTab
{
    Q_OBJECT

public:
    DriverDetailTab(DataBase *db, int id = 0, bool closable = false);

public slots:
    void resetQueryModel();
    void close();

private:
    int userID;
    QComboBox *selectPeriodCombo;
    QDateEdit *fromDate;
    QDateEdit *toDate;
    QPushButton *confirmPeriodButton;
    QLineEdit *userEditRO;
    QPushButton *closeButton;
};


class AddRouteTab : public QWidget
{
    Q_OBJECT

public:
    AddRouteTab(DataBase *db, int baseID = 0);

public slots:
    void fillFromBaseRoute();
    void addRoute();
    void close();

signals:
    void requestUpdateTable();

private:
    int baseRouteID;
    DataBase *db;
    QLineEdit *routeNameEdit;
    QLineEdit *fromEdit;
    QLineEdit *toEdit;
    QSpinBox *lengthSpin;
    QSpinBox *priceSpin;
    QSpinBox *driverFeeSpin;
    QLineEdit *descriptionEdit;
    QPushButton *addRouteButton;
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

signals:
    void requestUpdateTable();

private:
    DataBase *db;
    QueryModel *driversFirstModel;
    QueryModel *driversSecondModel;
    QueryModel *routesModel;
    QComboBox *firstDriverCombo;
    QSpinBox *firstDriverBonusSpin;
    QComboBox *secondDriverCombo;
    QSpinBox *secondDriverBonusSpin;
    QComboBox *routeCombo;
    QPushButton *addTranspButton;
    QPushButton *cancelButton;

};

#endif // MAINWINDOW_H
