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


void MainWindow::addTab(QWidget *tab, const QString &tabname)
{
    tabWidget->addTab(tab, tabname);
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
    LoginTab *tab = new LoginTab;
    addTab(tab, "Вход в программу");
    connect(tab, &LoginTab::userLoginAsLogist,
            this, &MainWindow::openTabsForLogist);
    connect(tab, &LoginTab::userLoginAsDriver,
            this, &MainWindow::openTabsForDriver);
    connect(tab, &LoginTab::userLoginAsAccounter,
            this, &MainWindow::openTabsForAccounter);
}


void MainWindow::openProfileTab(const QString &username, const QString &role)
{
    ProfileTab *tab = new ProfileTab(username, role);
    addTab(tab, "Профиль");
    connect(tab, &ProfileTab::userLogout,
            this, &MainWindow::resetTabs);
}


void MainWindow::openTabsForLogist(const QString &username)
{
    openProfileTab(username, "Логист");

    TransportationsTab *transp = new TransportationsTab;
    addTab(transp, "Перевозки");
    connect(transp, &TransportationsTab::requestAddTransportation,
            this, &MainWindow::openAddTransportationTab);

    RoutesTab *routes = new RoutesTab;
    addTab(routes, "Маршруты");
    connect(routes, &RoutesTab::requestAddRoute,
            this, &MainWindow::openAddRouteTab);
    connect(routes, &RoutesTab::requestEditRoute,
            this, &MainWindow::openEditRouteTab);
}


void MainWindow::openTabsForDriver(const QString &username)
{
    openProfileTab(username, "Водитель");
    addTab(new DriverDetailTab, "Перевозки");
}


void MainWindow::openTabsForAccounter(const QString &username)
{
    openProfileTab(username, "Бухгалтер");

    DriversTab *drivers = new DriversTab;
    connect(drivers, &DriversTab::requestDriverDetail,
            this, &MainWindow::openDriverDetailTab);

    LogistsTab *logists = new LogistsTab;
    AccountersTab *accounters = new AccountersTab;

    addTab(drivers, "Водители");
    addTab(logists, "Логисты");
    addTab(accounters, "Бухгалтеры");
}


void MainWindow::openDriverDetailTab(const QString &username)
{
    DriverDetailTab *tab = new DriverDetailTab;
    addTab(tab, username);
    tabWidget->setCurrentWidget(tab);
}


void MainWindow::openAddRouteTab()
{
    AddRouteTab *tab = new AddRouteTab;
    addTab(tab, "Добавление маршрута");
    tabWidget->setCurrentWidget(tab);
}


void MainWindow::openEditRouteTab()
{
    EditRouteTab *tab = new EditRouteTab;
    addTab(tab, "Редактирование маршрута");
    tabWidget->setCurrentWidget(tab);
}


void MainWindow::openAddTransportationTab()
{
    AddTransportationTab *tab = new AddTransportationTab;
    addTab(tab, "Добавление перевозки");
    tabWidget->setCurrentWidget(tab);
}

//==============================================================================

LoginTab::LoginTab()
{
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
        emit userLoginAsLogist(username);
        deleteLater();
    } else if(username == "a") {
        emit userLoginAsAccounter(username);
        deleteLater();
    } else if(username == "d") {
        emit userLoginAsDriver(username);
        deleteLater();
    } else {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("Введён неверный логин или пароль!");
        msgBox.setInformativeText(
            "Проверьте правильность ввода имени пользователя и пароля.\n"
            "Если вы уверены в правильности учётных данных, "
            "но не можете войти в систему, свяжитесь с администратором системы."
            );
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
    }
}

//==============================================================================

ProfileTab::ProfileTab(const QString &username, const QString &role)
{
    QFormLayout *tabLayout = new QFormLayout(this);

    QSpacerItem *verticalSpacer_1 = new QSpacerItem
        (0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
    tabLayout->addItem(verticalSpacer_1);

    QLabel *userLabel = new QLabel("Пользователь");
    tabLayout->addRow(userLabel);

    userEditRO = new QLineEdit;
    userEditRO->setReadOnly(true);
    tabLayout->addRow(userEditRO);
    userEditRO->setText(username);

    userRoleEditRO = new QLineEdit;
    userRoleEditRO->setReadOnly(true);
    tabLayout->addRow("Роль", userRoleEditRO);
    userRoleEditRO->setText(role);

    logoutButton = new QPushButton("Выйти из системы");
    tabLayout->addRow(logoutButton);
    connect(logoutButton, &QPushButton::clicked,
            this, &ProfileTab::logoutPressed);

    QFrame *line_1 = new QFrame;
    line_1->setFrameShape(QFrame::HLine);
    tabLayout->addRow(line_1);

    QLabel *changePasswordLabel = new QLabel("Сменить пароль");
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

    QSpacerItem *verticalSpacer_2 = new QSpacerItem
        (0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
    tabLayout->addItem(verticalSpacer_2);
}


void ProfileTab::logoutPressed()
{
    emit userLogout();
}

//==============================================================================

DriversTab::DriversTab()
{
    QFormLayout *tabLayout = new QFormLayout(this);

    table = new QTableView;
    tabLayout->addWidget(table);
    table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(table, &QTableView::customContextMenuRequested,
            this, &DriversTab::showTableContextMenu);
}


void DriversTab::showTableContextMenu(QPoint pos)
{
    QMenu menu;
    QAction *showTransp = new QAction("Просмотреть перевозки", this);
    menu.addAction(showTransp);

    QPoint formPos = mapToGlobal(pos);
    QAction *sel = menu.exec(formPos);
    if(sel == showTransp) {
        emit requestDriverDetail("example_driver");
    }
}

//==============================================================================

LogistsTab::LogistsTab()
{
    QFormLayout *tabLayout = new QFormLayout(this);

    table = new QTableView;
    tabLayout->addWidget(table);
    table->setContextMenuPolicy(Qt::CustomContextMenu);
}

//==============================================================================

AccountersTab::AccountersTab()
{
    QFormLayout *tabLayout = new QFormLayout(this);

    table = new QTableView;
    tabLayout->addWidget(table);
    table->setContextMenuPolicy(Qt::CustomContextMenu);
}

//==============================================================================

RoutesTab::RoutesTab()
{
    QFormLayout *tabLayout = new QFormLayout(this);

    table = new QTableView;
    tabLayout->addWidget(table);
    table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(table, &QTableView::customContextMenuRequested,
            this, &RoutesTab::showTableContextMenu);
}


void RoutesTab::showTableContextMenu(QPoint pos)
{
    QMenu menu;
    QAction *addRoute = new QAction("Добавить маршрут", this);
    menu.addAction(addRoute);
    QAction *editRoute = new QAction("Изменить маршрут", this);
    menu.addAction(editRoute);

    QPoint formPos = mapToGlobal(pos);
    QAction *sel = menu.exec(formPos);
    if(sel == addRoute) {
        emit requestAddRoute();
    } else if(sel == editRoute) {
        emit requestEditRoute();
    }
}

//==============================================================================

TransportationsTab::TransportationsTab()
{
    QFormLayout *tabLayout = new QFormLayout(this);

    table = new QTableView;
    tabLayout->addWidget(table);
    table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(table, &QTableView::customContextMenuRequested,
            this, &TransportationsTab::showTableContextMenu);
}


void TransportationsTab::showTableContextMenu(QPoint pos)
{
    QMenu menu;
    QAction *addTransp = new QAction("Добавить перевозку", this);
    menu.addAction(addTransp);

    QPoint formPos = mapToGlobal(pos);
    QAction *sel = menu.exec(formPos);
    if(sel == addTransp) {
        emit requestAddTransportation();
    }
}

//==============================================================================

DriverDetailTab::DriverDetailTab()
{
    QFormLayout *tabLayout = new QFormLayout(this);

    /*
    selectPeriodCombo = new QComboBox;
    tabLayout->addRow("Выберите период", selectPeriodCombo);

    QLabel *setPeriodLabel = new QLabel("Либо задайте его вручную");
    tabLayout->addRow(setPeriodLabel);
    */

    QLabel *setPeriodLabel = new QLabel("Задайте период");
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
    table->setContextMenuPolicy(Qt::CustomContextMenu);

    salaryForPeriodRO = new QLineEdit;
    salaryForPeriodRO->setReadOnly(true);
    tabLayout->addRow("Сумма за период", salaryForPeriodRO);

    QLabel *disregardLabel =
        new QLabel("Перевозки \"В работе\" при подсчёте не учитываются!");
    tabLayout->addRow(disregardLabel);
}

//==============================================================================

AddRouteTab::AddRouteTab()
{
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

EditRouteTab::EditRouteTab()
{
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

AddTransportationTab::AddTransportationTab()
{
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
