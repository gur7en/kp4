#include "mainwindow.h"
// #include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , tabWidget(NULL)
{
    central = new QWidget(this);
    setCentralWidget(central);
    layout = new QHBoxLayout(central);
    resize(640, 480);
    resetTabs();
}

MainWindow::~MainWindow()
{
}


void MainWindow::resetTabs()
{
    if(tabWidget) {
        delete tabWidget;
    }
    tabWidget = new QTabWidget;
    layout->addWidget(tabWidget);
    openLoginTab();
}


void MainWindow::openLoginTab()
{
    LoginTab *tab = new LoginTab(tabWidget);
    connect(tab, &LoginTab::userLoginAsLogist,
            this, &MainWindow::openTabsForLogist);
    connect(tab, &LoginTab::userLoginAsDriver,
            this, &MainWindow::openTabsForDriver);
    connect(tab, &LoginTab::userLoginAsAccounter,
            this, &MainWindow::openTabsForAccounter);
}


void MainWindow::openProfileTab()
{
    ProfileTab *tab = new ProfileTab(tabWidget);
    connect(tab, &ProfileTab::userLogout,
            this, &MainWindow::resetTabs);
}


void MainWindow::openTabsForLogist()
{
    openProfileTab();
    new RoutesTab(tabWidget);
    new TransportationsTab(tabWidget);
}


void MainWindow::openTabsForDriver()
{
    openProfileTab();
    new DriverDetailsTab(tabWidget);
}


void MainWindow::openTabsForAccounter()
{
    openProfileTab();
    new DriversTab(tabWidget);
    new LogistsTab(tabWidget);
    new AccountersTab(tabWidget);
}

//==============================================================================

LoginTab::LoginTab(QTabWidget *tabWidget)
{
    tabWidget->addTab(this, "Вход в программу");
    QFormLayout *tabLayout = new QFormLayout(this);

    QSpacerItem *verticalSpacer_1 = new QSpacerItem
        (0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
    tabLayout->addItem(verticalSpacer_1);

    loginEdit = new QLineEdit("");
    tabLayout->addRow("Логин", loginEdit);

    passwordEdit = new QLineEdit("");
    passwordEdit->setEchoMode(QLineEdit::Password);
    tabLayout->addRow("Пароль", passwordEdit);

    loginButton = new QPushButton("Вход");
    tabLayout->addRow(loginButton);
    connect(loginButton, &QPushButton::clicked,
            this, &LoginTab::loginPressed);

    QSpacerItem *verticalSpacer_2 = new QSpacerItem
        (0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
    tabLayout->addItem(verticalSpacer_2);
}


void LoginTab::loginPressed()
{
    QString username = loginEdit->text();
    if(username == "l") {
        emit userLoginAsLogist();
        deleteLater();
    } else if(username == "a") {
        emit userLoginAsAccounter();
        deleteLater();
    } else if(username == "d") {
        emit userLoginAsDriver();
        deleteLater();
    } else {
    }
}

//==============================================================================

ProfileTab::ProfileTab(QTabWidget *tabWidget)
{
    tabWidget->addTab(this, "Профиль");
    QFormLayout *tabLayout = new QFormLayout(this);

    QSpacerItem *verticalSpacer_1 = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
    tabLayout->addItem(verticalSpacer_1);

    userLabel = new QLabel("Пользователь");
    tabLayout->addRow(userLabel);

    userEditRO = new QLineEdit;
    userEditRO->setReadOnly(true);
    tabLayout->addRow(userEditRO);

    userRoleEditRO = new QLineEdit;
    userRoleEditRO->setReadOnly(true);
    tabLayout->addRow("Роль", userRoleEditRO);

    logoutButton = new QPushButton("Выйти из системы");
    tabLayout->addRow(logoutButton);
    connect(logoutButton, &QPushButton::clicked,
            this, &ProfileTab::logoutPressed);

    QFrame *line_1 = new QFrame;
    line_1->setFrameShape(QFrame::HLine);
    tabLayout->addRow(line_1);

    changePasswordLabel = new QLabel("Сменить пароль");
    tabLayout->addRow(changePasswordLabel);

    oldPasswordEdit = new QLineEdit;
    oldPasswordEdit->setEchoMode(QLineEdit::Password);
    tabLayout->addRow("Введите старый пароль", oldPasswordEdit);

    newPasswordEdit = new QLineEdit;
    newPasswordEdit->setEchoMode(QLineEdit::Password);
    tabLayout->addRow("Введите новый пароль", newPasswordEdit);

    repeatPasswordEdit = new QLineEdit;
    repeatPasswordEdit->setEchoMode(QLineEdit::Password);
    tabLayout->addRow("Повторите новый пароль", repeatPasswordEdit);

    changePasswordButton = new QPushButton("Сменить пароль");
    tabLayout->addRow(changePasswordButton);

    QSpacerItem *verticalSpacer_2 = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
    tabLayout->addItem(verticalSpacer_2);
}


void ProfileTab::logoutPressed()
{
    emit userLogout();
}

//==============================================================================

DriversTab::DriversTab(QTabWidget *tabWidget)
{
    tabWidget->addTab(this, "Водители");
    QFormLayout *tabLayout = new QFormLayout(this);

    table = new QTableView;
    tabLayout->addWidget(table);
}

//==============================================================================

LogistsTab::LogistsTab(QTabWidget *tabWidget)
{
    tabWidget->addTab(this, "Логисты");
    QFormLayout *tabLayout = new QFormLayout(this);

    table = new QTableView;
    tabLayout->addWidget(table);
}

//==============================================================================

AccountersTab::AccountersTab(QTabWidget *tabWidget)
{
    tabWidget->addTab(this, "Бухгалтеры");
    QFormLayout *tabLayout = new QFormLayout(this);

    table = new QTableView;
    tabLayout->addWidget(table);
}

//==============================================================================

RoutesTab::RoutesTab(QTabWidget *tabWidget)
{
    tabWidget->addTab(this, "Маршруты");
    QFormLayout *tabLayout = new QFormLayout(this);

    table = new QTableView;
    tabLayout->addWidget(table);
}

//==============================================================================

TransportationsTab::TransportationsTab(QTabWidget *tabWidget)
{
    tabWidget->addTab(this, "Перевозки");
    QFormLayout *tabLayout = new QFormLayout(this);

    table = new QTableView;
    tabLayout->addWidget(table);
}

//==============================================================================

DriverDetailsTab::DriverDetailsTab(QTabWidget *tabWidget)
{
    tabWidget->addTab(this, "Перевозки");
    QFormLayout *tabLayout = new QFormLayout(this);

    selectPeriodCombo = new QComboBox;
    tabLayout->addRow("Выберите период", selectPeriodCombo);

    QLabel *setPeriodLabel = new QLabel("Либо задайте его вручную");
    tabLayout->addRow(setPeriodLabel);

    fromDate = new QDateEdit;
    tabLayout->addRow("От", fromDate);

    toDate = new QDateEdit;
    tabLayout->addRow("До", toDate);

    confirmPeriodButton = new QPushButton("Подтвердить период");
    tabLayout->addRow(confirmPeriodButton);

    QFrame *line_1 = new QFrame();
    line_1->setFrameShape(QFrame::HLine);
    tabLayout->addRow(line_1);

    userEditRO = new QLineEdit;
    userEditRO->setReadOnly(true);
    tabLayout->addRow("Водитель", userEditRO);

    table = new QTableView;
    tabLayout->addRow(table);

    salaryForPeriodRO = new QLineEdit;
    salaryForPeriodRO->setReadOnly(true);
    tabLayout->addRow("Сумма за период", salaryForPeriodRO);

    QLabel *disregardLabel =
        new QLabel("Перевозки \"В работе\" при подсчёте не учитываются!");
    tabLayout->addRow(disregardLabel);
}

//==============================================================================

AddRouteTab::AddRouteTab(QTabWidget *tabWidget)
{
    tabWidget->addTab(this, "Добавление маршрута");
    QFormLayout *tabLayout = new QFormLayout(this);

    QSpacerItem *verticalSpacer_1 = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
    tabLayout->addItem(verticalSpacer_1);

    routeNameEdit = new QLineEdit("");
    tabLayout->addRow("Название", routeNameEdit);

    fromEdit = new QLineEdit("");
    tabLayout->addRow("Откуда", fromEdit);

    toEdit = new QLineEdit("");
    tabLayout->addRow("Куда", toEdit);

    lengthSpin = new QDoubleSpinBox();
    tabLayout->addRow("Длина (км)", lengthSpin);

    priceSpin = new QSpinBox();
    tabLayout->addRow("Цена (руб)", priceSpin);

    descriptionEdit = new QLineEdit("");
    tabLayout->addRow("Описание", descriptionEdit);

    addRouteButton = new QPushButton("Добавить маршрут");
    tabLayout->addRow(addRouteButton);

    cancelButton = new QPushButton("Отмена");
    tabLayout->addRow(cancelButton);
    connect(cancelButton, &QPushButton::clicked,
            this, &AddRouteTab::close);

    QSpacerItem *verticalSpacer_2 = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
    tabLayout->addItem(verticalSpacer_2);
}


void AddRouteTab::close()
{
    deleteLater();
}

//==============================================================================

EditRouteTab::EditRouteTab(QTabWidget *tabWidget)
{
    tabWidget->addTab(this, "Редактирование маршрута");
    QFormLayout *tabLayout = new QFormLayout(this);

    QSpacerItem *verticalSpacer_1 = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
    tabLayout->addItem(verticalSpacer_1);

    routeNameEdit = new QLineEdit;
    tabLayout->addRow("Название", routeNameEdit);

    fromEdit = new QLineEdit;
    tabLayout->addRow("Откуда", fromEdit);

    toEdit = new QLineEdit;
    tabLayout->addRow("Куда", toEdit);

    lengthSpin = new QDoubleSpinBox;
    tabLayout->addRow("Длина (км)", lengthSpin);

    priceSpin = new QSpinBox;
    tabLayout->addRow("Цена (руб)", priceSpin);

    descriptionEdit = new QLineEdit;
    tabLayout->addRow("Описание", descriptionEdit);

    confirmButton = new QPushButton("Сохранить изменения");
    tabLayout->addRow(confirmButton);

    cancelButton = new QPushButton("Отмена");
    tabLayout->addRow(cancelButton);
    connect(cancelButton, &QPushButton::clicked,
            this, &EditRouteTab::close);

    QSpacerItem *verticalSpacer_2 = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
    tabLayout->addItem(verticalSpacer_2);
}


void EditRouteTab::close()
{
    deleteLater();
}

//==============================================================================

AddTransportationTab::AddTransportationTab(QTabWidget *tabWidget)
{
    tabWidget->addTab(this, "Добавление перевозки");
    QFormLayout *tabLayout = new QFormLayout(this);

    QSpacerItem *verticalSpacer_1 = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
    tabLayout->addItem(verticalSpacer_1);

    firstDriverCombo = new QComboBox;
    tabLayout->addRow("Водитель", firstDriverCombo);

    firstDriverBonusSpin = new QSpinBox;
    tabLayout->addRow("Премия (руб)", firstDriverBonusSpin);

    QFrame *line_1 = new QFrame();
    line_1->setFrameShape(QFrame::HLine);
    tabLayout->addRow(line_1);

    secondDriverCombo = new QComboBox;
    tabLayout->addRow("Второй водитель", secondDriverCombo);

    secondDriverBonusSpin = new QSpinBox;
    tabLayout->addRow("Премия (руб)", secondDriverBonusSpin);

    QFrame *line_2 = new QFrame();
    line_2->setFrameShape(QFrame::HLine);
    tabLayout->addRow(line_2);

    routeCombo = new QComboBox;
    tabLayout->addRow("Маршрут", routeCombo);

    QFrame *line_3 = new QFrame();
    line_3->setFrameShape(QFrame::HLine);
    tabLayout->addRow(line_3);

    addTranspButton = new QPushButton("Добавить перевозку");
    tabLayout->addRow(addTranspButton);

    cancelButton = new QPushButton("Отмена");
    tabLayout->addRow(cancelButton);
    connect(cancelButton, &QPushButton::clicked,
            this, &AddTransportationTab::close);

    QSpacerItem *verticalSpacer_2 = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
    tabLayout->addItem(verticalSpacer_2);
}


void AddTransportationTab::close()
{
    deleteLater();
}
