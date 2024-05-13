#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent, DataBase *db)
    : QMainWindow(parent)
    , tabWidget(NULL)
    , db(db)
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
    db->logout();
    tabWidget = new QTabWidget;
    layout->addWidget(tabWidget);
    openLoginTab();
}


void MainWindow::openLoginTab()
{
    LoginTab *tab = new LoginTab(db);
    addTab(tab, "Вход в программу");
    connect(tab, &LoginTab::userLoginAsLogist,
            this, &MainWindow::openTabsForLogist);
    connect(tab, &LoginTab::userLoginAsDriver,
            this, &MainWindow::openTabsForDriver);
    connect(tab, &LoginTab::userLoginAsAccounter,
            this, &MainWindow::openTabsForAccounter);
}


void MainWindow::openProfileTab()
{
    ProfileTab *tab = new ProfileTab(db);
    addTab(tab, "Профиль");
    connect(tab, &ProfileTab::userLogout,
            this, &MainWindow::resetTabs);
}


void MainWindow::openTabsForLogist()
{
    openProfileTab();

    TransportationsTab *transp = new TransportationsTab(db);
    addTab(transp, "Перевозки");
    connect(transp, &TransportationsTab::requestAddTransportation,
            this, &MainWindow::openAddTransportationTab);

    RoutesTab *routes = new RoutesTab(db);
    addTab(routes, "Маршруты");
    connect(routes, &RoutesTab::requestAddRoute,
            this, &MainWindow::openAddRouteTab);
    connect(routes, &RoutesTab::requestEditRoute,
            this, &MainWindow::openEditRouteTab);
}


void MainWindow::openTabsForDriver()
{
    openProfileTab();
    addTab(new DriverDetailTab(db), "Перевозки");
}


void MainWindow::openTabsForAccounter()
{
    openProfileTab();

    DriversTab *drivers = new DriversTab(db);
    drivers->resetQueryModel();
    connect(drivers, &DriversTab::requestDriverDetail,
            this, &MainWindow::openDriverDetailTab);

    LogistsTab *logists = new LogistsTab(db);
    logists->resetQueryModel();

    AccountersTab *accounters = new AccountersTab(db);
    accounters->resetQueryModel();

    addTab(drivers, "Водители");
    addTab(logists, "Логисты");
    addTab(accounters, "Бухгалтеры");
}


void MainWindow::openDriverDetailTab()
{
    DriverDetailTab *tab = new DriverDetailTab(db);
    addTab(tab, db->getUserShortName());
    tabWidget->setCurrentWidget(tab);
}


void MainWindow::openAddRouteTab()
{
    AddRouteTab *tab = new AddRouteTab(db);
    addTab(tab, "Добавление маршрута");
    tabWidget->setCurrentWidget(tab);
}


void MainWindow::openEditRouteTab()
{
    EditRouteTab *tab = new EditRouteTab(db);
    addTab(tab, "Редактирование маршрута");
    tabWidget->setCurrentWidget(tab);
}


void MainWindow::openAddTransportationTab()
{
    AddTransportationTab *tab = new AddTransportationTab(db);
    addTab(tab, "Добавление перевозки");
    tabWidget->setCurrentWidget(tab);
}

//==============================================================================

UserListTab::UserListTab(DataBase *db)
{
    this->db = db;

    QFormLayout *tabLayout = new QFormLayout(this);

    tableModel = new QSqlQueryModel;

    table = new QTableView;
    tabLayout->addWidget(table);
    table->setContextMenuPolicy(Qt::CustomContextMenu);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setModel(tableModel);
    connect(table, &QTableView::customContextMenuRequested,
            this, &UserListTab::showTableContextMenu);

    tableContextMenu = new QMenu;
    updateTableAction = new QAction("Обновить", this);
    tableContextMenu->addAction(updateTableAction);

    connect(this, &UserListTab::requestUpdateTable,
            this, &UserListTab::resetQueryModel);
}


void UserListTab::showTableContextMenu(QPoint position)
{
    QPoint formPos = mapToGlobal(position);
    QAction *selectedAction = tableContextMenu->exec(formPos);
    actionTableContextMenu(selectedAction);
}


bool UserListTab::actionTableContextMenu(QAction *selected)
{
    if(selected == updateTableAction) {
        emit requestUpdateTable();
    } else {
        return false;
    }
    return true;
}


void UserListTab::resetQueryModel()
{
}

//==============================================================================

LoginTab::LoginTab(DataBase *db)
{
    this->db = db;

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
    QString password = passwordEdit->text();
    bool success_login = db->login(username, password);
    if(!success_login) {
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
        return;
    }

    switch(db->getUserRole()) {
    case UserRole::Driver:
        emit userLoginAsDriver();
        break;
    case UserRole::Logist:
        emit userLoginAsLogist();
        break;
    case UserRole::Accounter:
        emit userLoginAsAccounter();
        break;
    default:
        return;
    }

    deleteLater();
}

//==============================================================================

ProfileTab::ProfileTab(DataBase *db)
{
    this->db = db;

    QFormLayout *tabLayout = new QFormLayout(this);

    QSpacerItem *verticalSpacer_1 = new QSpacerItem
        (0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
    tabLayout->addItem(verticalSpacer_1);

    QLabel *userLabel = new QLabel("Пользователь");
    tabLayout->addRow(userLabel);

    userEditRO = new QLineEdit;
    userEditRO->setReadOnly(true);
    tabLayout->addRow(userEditRO);
    userEditRO->setText(db->getUserFullName());

    userRoleEditRO = new QLineEdit;
    userRoleEditRO->setReadOnly(true);
    tabLayout->addRow("Роль", userRoleEditRO);
    // userRoleEditRO->setText(db->getUserRole());

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

DriversTab::DriversTab(DataBase *db)
    : UserListTab(db)
{
    showTranspMenuAction = new QAction("Просмотреть перевозки", this);
    tableContextMenu->addAction(showTranspMenuAction);
}


void DriversTab::showTableContextMenu(QPoint pos)
{
    QPoint formPos = mapToGlobal(pos);
    QAction *selectedAction = tableContextMenu->exec(formPos);
    bool actionPerformed = actionTableContextMenu(selectedAction);
    if(!actionPerformed) {
        UserListTab::actionTableContextMenu(selectedAction);
    }
}


bool DriversTab::actionTableContextMenu(QAction *selected)
{
    if(selected == showTranspMenuAction) {
        emit requestDriverDetail("example_driver");
    } else {
        return false;
    }
    return true;
}


void DriversTab::resetQueryModel()
{
    tableModel->setQuery("SELECT * FROM users WHERE role='driver';");
}

//==============================================================================

LogistsTab::LogistsTab(DataBase *db)
    : UserListTab(db)
{
}


void LogistsTab::resetQueryModel()
{
    tableModel->setQuery("SELECT * FROM users WHERE role='logist';");
}

//==============================================================================

AccountersTab::AccountersTab(DataBase *db)
    : UserListTab(db)
{
}


void AccountersTab::resetQueryModel()
{
    tableModel->setQuery("SELECT * FROM users WHERE role='accounter';");
}

//==============================================================================

RoutesTab::RoutesTab(DataBase *db)
{
    this->db = db;

    QFormLayout *tabLayout = new QFormLayout(this);

    table = new QTableView;
    /*
     * TODO
    table->setModel();
    table->show();
    */
    tabLayout->addWidget(table);
    table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(table, &QTableView::customContextMenuRequested,
            this, &RoutesTab::showTableContextMenu);

    tableSelectionModel = table->selectionModel();

    tableContextMenu = new QMenu;
    addRouteMenuAction = new QAction("Добавить маршрут", this);
    editRouteMenuAction = new QAction("Изменить маршрут", this);
}


void RoutesTab::showTableContextMenu(QPoint pos)
{
    tableContextMenu->addAction(addRouteMenuAction);
    if(tableSelectionModel->hasSelection()) {
        tableContextMenu->addAction(editRouteMenuAction);
    }

    QPoint formPos = mapToGlobal(pos);
    QAction *selectedAction = tableContextMenu->exec(formPos);
    if(selectedAction == addRouteMenuAction) {
        emit requestAddRoute();
    } else if(selectedAction == editRouteMenuAction) {
        emit requestEditRoute();
    }
}

//==============================================================================

TransportationsTab::TransportationsTab(DataBase *db)
{
    this->db = db;

    QFormLayout *tabLayout = new QFormLayout(this);

    table = new QTableView;
    tabLayout->addWidget(table);
    table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(table, &QTableView::customContextMenuRequested,
            this, &TransportationsTab::showTableContextMenu);

    tableContextMenu = new QMenu;
    addTranspMenuAction = new QAction("Добавить перевозку", this);
}


void TransportationsTab::showTableContextMenu(QPoint pos)
{
    tableContextMenu->addAction(addTranspMenuAction);

    QPoint formPos = mapToGlobal(pos);
    QAction *selectedAction = tableContextMenu->exec(formPos);
    if(selectedAction == addTranspMenuAction) {
        emit requestAddTransportation();
    }
}

//==============================================================================

DriverDetailTab::DriverDetailTab(DataBase *db)
{
    this->db = db;

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

AddRouteTab::AddRouteTab(DataBase *db)
{
    this->db = db;

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

EditRouteTab::EditRouteTab(DataBase *db)
{
    this->db = db;

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

AddTransportationTab::AddTransportationTab(DataBase *db)
{
    this->db = db;

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
