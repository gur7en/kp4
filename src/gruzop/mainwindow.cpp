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
    db->logout();
    db->disconnect();
    if(tabWidget) {
        delete tabWidget;
    }
    tabWidget = new QTabWidget;
    layout->addWidget(tabWidget);
    openLoginTab();
}


void MainWindow::openLoginTab()
{
    db->reconnect("gestart", "");
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
    connect(drivers, &DriversTab::requestDriverDetail,
            this, &MainWindow::openDriverDetailTab);

    LogistsTab *logists = new LogistsTab(db);

    AccountersTab *accounters = new AccountersTab(db);

    addTab(drivers, "Водители");
    addTab(logists, "Логисты");
    addTab(accounters, "Бухгалтеры");
}


void MainWindow::openDriverDetailTab(int userID)
{
    DriverDetailTab *tab = new DriverDetailTab(db, userID, true);
    addTab(tab, "Перевозки водителя");
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

GeneralizedListTab::GeneralizedListTab(DataBase *db)
{
    this->db = db;

    tabLayout = new QFormLayout(this);

    tableModel = new QSqlQueryModel;

    table = new QTableView;
    table->setContextMenuPolicy(Qt::CustomContextMenu);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setModel(tableModel);
    connect(table, &QTableView::customContextMenuRequested,
            this, &GeneralizedListTab::showTableContextMenu);

    tableContextMenu = new QMenu;

    updateTableAction = new QAction("Обновить", this);
    tableContextMenu->addAction(updateTableAction);
    connect(updateTableAction, &QAction::triggered,
            this, &GeneralizedListTab::resetQueryModel);
}


void GeneralizedListTab::showTableContextMenu(QPoint position)
{
    QPoint formPos = table->viewport()->mapToGlobal(position);
    tableContextMenu->popup(formPos);
}


int GeneralizedListTab::selectedID()
{
    QItemSelectionModel *select = table->selectionModel();
    QModelIndex index = select->currentIndex();
    return index.sibling(index.row(), 0).data().toInt();
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

    UserRole::Code role;
    role = db->login(username, password);

    if(role == UserRole::Unlogin) {
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
    } else if(role == UserRole::Unknown) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setText("Неизвестный тип учётной записи!");
        msgBox.setInformativeText("Свяжитесь с администратором системы.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    switch(role) {
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
        break;
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
    userEditRO->setText(db->userFullName());

    userRoleEditRO = new QLineEdit;
    userRoleEditRO->setReadOnly(true);
    tabLayout->addRow("Роль", userRoleEditRO);
    switch(db->userRole()) {
    case UserRole::Driver:
        userRoleEditRO->setText("Водитель");
        break;
    case UserRole::Logist:
        userRoleEditRO->setText("Логист");
        break;
    case UserRole::Accounter:
        userRoleEditRO->setText("Бухгалтер");
        break;
    default:
        break;
    }

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
    : GeneralizedListTab(db)
{
    tabLayout->addWidget(table);

    showTranspMenuAction = new QAction("Просмотреть перевозки", this);
    tableContextMenu->addAction(showTranspMenuAction);
    connect(showTranspMenuAction, &QAction::triggered,
            this, &DriversTab::detailSelectedDriver);

    DriversTab::resetQueryModel();
}


void DriversTab::showTableContextMenu(QPoint position)
{
    tableContextMenu->removeAction(showTranspMenuAction);
    QItemSelectionModel *select = table->selectionModel();
    if(select->hasSelection()) {
        tableContextMenu->addAction(showTranspMenuAction);
    }

    QPoint formPos = table->viewport()->mapToGlobal(position);
    tableContextMenu->popup(formPos);
}


void DriversTab::detailSelectedDriver()
{
    emit requestDriverDetail(selectedID());
}


void DriversTab::resetQueryModel()
{
    tableModel->setQuery(db->usersQuery(UserRole::Driver));
    table->resizeColumnsToContents();
    table->setColumnHidden(0, true);
}

//==============================================================================

LogistsTab::LogistsTab(DataBase *db)
    : GeneralizedListTab(db)
{
    tabLayout->addWidget(table);

    LogistsTab::resetQueryModel();
}


void LogistsTab::resetQueryModel()
{
    tableModel->setQuery(db->usersQuery(UserRole::Logist));
    table->resizeColumnsToContents();
    table->setColumnHidden(0, true);
}

//==============================================================================

AccountersTab::AccountersTab(DataBase *db)
    : GeneralizedListTab(db)
{
    tabLayout->addWidget(table);

    AccountersTab::resetQueryModel();
}


void AccountersTab::resetQueryModel()
{
    tableModel->setQuery(db->usersQuery(UserRole::Accounter));
    table->resizeColumnsToContents();
    table->setColumnHidden(0, true);
}

//==============================================================================

RoutesTab::RoutesTab(DataBase *db)
    : GeneralizedListTab(db)
{
    tabLayout->addWidget(table);

    addRouteMenuAction = new QAction("Добавить маршрут", this);
    tableContextMenu->addAction(addRouteMenuAction);
    connect(addRouteMenuAction, &QAction::triggered,
            this, &RoutesTab::requestAddRoute);

    editRouteMenuAction = new QAction("Изменить маршрут", this);
    tableContextMenu->addAction(editRouteMenuAction);
    connect(editRouteMenuAction, &QAction::triggered,
            this, &RoutesTab::requestEditRoute);

    RoutesTab::resetQueryModel();
}


void RoutesTab::showTableContextMenu(QPoint position)
{
    tableContextMenu->removeAction(editRouteMenuAction);
    QItemSelectionModel *select = table->selectionModel();
    if(select->hasSelection()) {
        tableContextMenu->addAction(editRouteMenuAction);
    }

    QPoint formPos = table->viewport()->mapToGlobal(position);
    tableContextMenu->popup(formPos);
}


void RoutesTab::resetQueryModel()
{
    tableModel->setQuery(db->routesQueryAll());
    table->resizeColumnsToContents();
    table->setColumnHidden(0, true);
}

//==============================================================================

TransportationsTab::TransportationsTab(DataBase *db)
    : GeneralizedListTab(db)
{
    tabLayout->addWidget(table);

    addTranspMenuAction = new QAction("Добавить перевозку", this);
    tableContextMenu->addAction(addTranspMenuAction);
    connect(addTranspMenuAction, &QAction::triggered,
            this, &TransportationsTab::requestAddTransportation);

    successTranspMenuAction = new QAction("Завершить перевозку", this);
    tableContextMenu->addAction(successTranspMenuAction);
    connect(successTranspMenuAction, &QAction::triggered,
            this, &TransportationsTab::successTransportation);

    cancelTranspMenuAction = new QAction("Отменить перевозку", this);
    tableContextMenu->addAction(cancelTranspMenuAction);
    connect(cancelTranspMenuAction, &QAction::triggered,
            this, &TransportationsTab::cancelTransportation);

    reopenTranspMenuAction = new QAction("Вернуть перевозку в работу", this);
    tableContextMenu->addAction(reopenTranspMenuAction);
    connect(reopenTranspMenuAction, &QAction::triggered,
            this, &TransportationsTab::reopenTransportation);

    TransportationsTab::resetQueryModel();
}


void TransportationsTab::successTransportation()
{
    db->successTranspQuery(selectedID());
    TransportationsTab::resetQueryModel();
}


void TransportationsTab::cancelTransportation()
{
    db->cancelTranspQuery(selectedID());
    TransportationsTab::resetQueryModel();
}


void TransportationsTab::reopenTransportation()
{
    db->reopenTranspQuery(selectedID());
    TransportationsTab::resetQueryModel();
}


void TransportationsTab::resetQueryModel()
{
    tableModel->setQuery(db->transpQuery());
    table->resizeColumnsToContents();
    table->setColumnHidden(0, true);
}

//==============================================================================

DriverDetailTab::DriverDetailTab(DataBase *db, int id, bool closable)
    : GeneralizedListTab(db)
{
    if(id == 0) {
        id = db->userID();
    }

    userID = id;

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

    if(closable) {
        userEditRO = new QLineEdit;
        userEditRO->setReadOnly(true);
        userEditRO->setText(db->userFullName(userID));
        tabLayout->addRow("Водитель", userEditRO);
    } else {
        userEditRO = nullptr;
    }

    tabLayout->addRow(table);

    salaryForPeriodRO = new QLineEdit;
    salaryForPeriodRO->setReadOnly(true);
    tabLayout->addRow("Сумма за период", salaryForPeriodRO);

    QLabel *disregardLabel =
        new QLabel("Перевозки \"В работе\" при подсчёте не учитываются!");
    tabLayout->addRow(disregardLabel);

    if(closable) {
        closeButton = new QPushButton("Закрыть");
        tabLayout->addRow(closeButton);
        connect(closeButton, &QPushButton::clicked,
                this, &DriverDetailTab::close);
    }

    DriverDetailTab::resetQueryModel();
}


void DriverDetailTab::resetQueryModel()
{
    tableModel->setQuery(db->driverTranspQuery(userID));
    table->resizeColumnsToContents();
    table->setColumnHidden(0, true);
}


void DriverDetailTab::close()
{
    deleteLater();
}

//==============================================================================

AddRouteTab::AddRouteTab(DataBase *db)
{
    this->db = db;

    QFormLayout *tabLayout = new QFormLayout(this);

    QSpacerItem *verticalSpacer_1 = new QSpacerItem
        (0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
    tabLayout->addItem(verticalSpacer_1);

    routeNameEdit = new QLineEdit("");
    tabLayout->addRow("Название", routeNameEdit);

    fromEdit = new QLineEdit("");
    tabLayout->addRow("Откуда", fromEdit);

    toEdit = new QLineEdit("");
    tabLayout->addRow("Куда", toEdit);

    lengthSpin = new QDoubleSpinBox();
    lengthSpin->setRange(0, 9999);
    tabLayout->addRow("Длина (км)", lengthSpin);

    priceSpin = new QSpinBox();
    priceSpin->setRange(0, 9999);
    tabLayout->addRow("Цена для клиента (руб)", priceSpin);

    driverFeeSpin = new QSpinBox();
    driverFeeSpin->setRange(0, 9999);
    tabLayout->addRow("Оплата водителю (руб)", driverFeeSpin);

    descriptionEdit = new QLineEdit("");
    tabLayout->addRow("Описание", descriptionEdit);

    addRouteButton = new QPushButton("Добавить маршрут");
    tabLayout->addRow(addRouteButton);
    connect(addRouteButton, &QPushButton::clicked,
            this, &AddRouteTab::addRoute);

    cancelButton = new QPushButton("Отмена");
    tabLayout->addRow(cancelButton);
    connect(cancelButton, &QPushButton::clicked,
            this, &AddRouteTab::close);

    QSpacerItem *verticalSpacer_2 = new QSpacerItem
        (0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
    tabLayout->addItem(verticalSpacer_2);
}


void AddRouteTab::addRoute()
{
    QString name = routeNameEdit->text();
    QString start = fromEdit->text();
    QString end = toEdit->text();
    int length = lengthSpin->value();
    QString details = descriptionEdit->text();
    int clientPrice = priceSpin->value();
    int driverFee = driverFeeSpin->value();

    QSqlQuery query;
    query = db->addRouteQuery(name, start, end, length, details,
                              clientPrice, driverFee);
    close();
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

    QSpacerItem *verticalSpacer_1 = new QSpacerItem
        (0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
    tabLayout->addItem(verticalSpacer_1);

    routeNameEdit = new QLineEdit;
    tabLayout->addRow("Название", routeNameEdit);

    fromEdit = new QLineEdit;
    tabLayout->addRow("Откуда", fromEdit);

    toEdit = new QLineEdit;
    tabLayout->addRow("Куда", toEdit);

    lengthSpin = new QDoubleSpinBox;
    tabLayout->addRow("Длина (км)", lengthSpin);

    priceSpin = new QSpinBox();
    priceSpin->setRange(0, 9999);
    tabLayout->addRow("Цена для клиента (руб)", priceSpin);

    driverFeeSpin = new QSpinBox();
    driverFeeSpin->setRange(0, 9999);
    tabLayout->addRow("Оплата водителю (руб)", driverFeeSpin);

    descriptionEdit = new QLineEdit;
    tabLayout->addRow("Описание", descriptionEdit);

    confirmButton = new QPushButton("Сохранить изменения");
    tabLayout->addRow(confirmButton);
    connect(confirmButton, &QPushButton::clicked,
            this, &EditRouteTab::editRoute);

    cancelButton = new QPushButton("Отмена");
    tabLayout->addRow(cancelButton);
    connect(cancelButton, &QPushButton::clicked,
            this, &EditRouteTab::close);

    QSpacerItem *verticalSpacer_2 = new QSpacerItem
        (0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
    tabLayout->addItem(verticalSpacer_2);
}


void EditRouteTab::editRoute()
{
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

    QSpacerItem *verticalSpacer_1 = new QSpacerItem
        (0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
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
    connect(addTranspButton, &QPushButton::clicked,
            this, &AddTransportationTab::addTransportation);

    cancelButton = new QPushButton("Отмена");
    tabLayout->addRow(cancelButton);
    connect(cancelButton, &QPushButton::clicked,
            this, &AddTransportationTab::close);

    QSpacerItem *verticalSpacer_2 = new QSpacerItem
        (0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
    tabLayout->addItem(verticalSpacer_2);
}


void AddTransportationTab::addTransportation()
{
    int firstDriverID;
    int firstDriverBonus;
    int secondDriverID;
    int secondDriverBonus;
    int route;
}


void AddTransportationTab::close()
{
    deleteLater();
}
