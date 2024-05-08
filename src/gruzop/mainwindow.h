#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void resetTabs();
    void openLoginTab();
    void openProfileTab();
    void openTabsForLogist();
    void openTabsForDriver();
    void openTabsForAccounter();

private:
    QWidget *central;
    QHBoxLayout *layout;
    QTabWidget *tabWidget;
};


class LoginTab : public QWidget
{
    Q_OBJECT

public:
    LoginTab(QTabWidget *tabWidget);

signals:
    void userLoginAsLogist();
    void userLoginAsDriver();
    void userLoginAsAccounter();

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
    ProfileTab(QTabWidget *tabWidget);

signals:
    void userLogout();

private:
    QLabel *userLabel;
    QLineEdit *userEditRO;
    QLineEdit *userRoleEditRO;
    QPushButton *logoutButton;
    QLabel *changePasswordLabel;
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
    DriversTab(QTabWidget *tabWidget);

private:
    QTableView *table;
};


class LogistsTab : public QWidget
{
    Q_OBJECT

public:
    LogistsTab(QTabWidget *tabWidget);

private:
    QTableView *table;
};


class AccountersTab : public QWidget
{
    Q_OBJECT

public:
    AccountersTab(QTabWidget *tabWidget);

private:
    QTableView *table;
};


class RoutesTab : public QWidget
{
    Q_OBJECT

public:
    RoutesTab(QTabWidget *tabWidget);

private:
    QTableView *table;
};


class TransportationsTab : public QWidget
{
    Q_OBJECT

public:
    TransportationsTab(QTabWidget *tabWidget);

private:
    QTableView *table;
};


class DriverDetailsTab : public QWidget
{
    Q_OBJECT

public:
    DriverDetailsTab(QTabWidget *tabWidget);

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
    AddRouteTab(QTabWidget *tabWidget);

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
    EditRouteTab(QTabWidget *tabWidget);

public slots:
    void close();

private:
    QFormLayout *layout;
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
    AddTransportationTab(QTabWidget *tabWidget);

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
