#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QMenu>

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


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void openLoginTab();
    void openProfileTab(const QString &username, const QString &role);
    void openTabsForLogist(const QString &username);
    void openTabsForDriver(const QString &username);
    void openTabsForAccounter(const QString &username);
    void openDriverDetailTab(const QString &username);
    void openAddRouteTab();
    void openEditRouteTab();
    void openAddTransportationTab();

private:
    QWidget *central;
    QHBoxLayout *layout;
    QTabWidget *tabWidget;

    void addTab(QWidget *tab, const QString &tabname);
    void resetTabs();
};


class LoginTab : public QWidget
{
    Q_OBJECT

public:
    LoginTab();

signals:
    void userLoginAsLogist(const QString &username);
    void userLoginAsDriver(const QString &username);
    void userLoginAsAccounter(const QString &username);

private:
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
    ProfileTab(const QString &username, const QString &role);

signals:
    void userLogout();

private:
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


class DriversTab : public QWidget
{
    Q_OBJECT

public:
    DriversTab();

public slots:
    void showTableContextMenu(QPoint pos);

signals:
    void requestDriverDetail(const QString &username);

private:
    QTableView *table;
};


class LogistsTab : public QWidget
{
    Q_OBJECT

public:
    LogistsTab();

private:
    QTableView *table;
};


class AccountersTab : public QWidget
{
    Q_OBJECT

public:
    AccountersTab();

private:
    QTableView *table;
};


class RoutesTab : public QWidget
{
    Q_OBJECT

public:
    RoutesTab();

public slots:
    void showTableContextMenu(QPoint pos);

signals:
    void requestAddRoute();
    void requestEditRoute();

private:
    QTableView *table;
};


class TransportationsTab : public QWidget
{
    Q_OBJECT

public:
    TransportationsTab();

public slots:
    void showTableContextMenu(QPoint pos);

signals:
    void requestAddTransportation();
    void requestEditTransportation();

private:
    QTableView *table;
};


class DriverDetailTab : public QWidget
{
    Q_OBJECT

public:
    DriverDetailTab();

private:
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
    AddRouteTab();

public slots:
    void close();

private:
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
    EditRouteTab();

public slots:
    void close();

private:
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
    AddTransportationTab();

public slots:
    void close();

private:
    QComboBox *firstDriverCombo;
    QSpinBox *firstDriverBonusSpin;
    QComboBox *secondDriverCombo;
    QSpinBox *secondDriverBonusSpin;
    QComboBox *routeCombo;
    QPushButton *addTranspButton;
    QPushButton *cancelButton;
};

#endif // MAINWINDOW_H
