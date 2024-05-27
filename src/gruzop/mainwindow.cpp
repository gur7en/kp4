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
    connect(routes, &RoutesTab::requestAddBasedRoute,
            this, &MainWindow::openAddRouteTab);
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


void MainWindow::openAddRouteTab(GeneralizedTableTab *requester, int baseRouteID)
{
    AddRouteTab *tab = new AddRouteTab(db, baseRouteID);
    addTab(tab, "Добавление маршрута");
    tabWidget->setCurrentWidget(tab);
    connect(tab, &AddRouteTab::requestUpdateTable,
            requester, &GeneralizedTableTab::resetQueryModel);
}


void MainWindow::openAddTransportationTab(GeneralizedTableTab *requester)
{
    AddTransportationTab *tab = new AddTransportationTab(db);
    addTab(tab, "Добавление перевозки");
    tabWidget->setCurrentWidget(tab);
    connect(tab, &AddTransportationTab::requestUpdateTable,
            requester, &GeneralizedTableTab::resetQueryModel);
}

//==============================================================================

GeneralizedTableTab::GeneralizedTableTab(DataBase *db)
{
    this->db = db;

    tabLayout = new QFormLayout(this);

    tableModel = new QueryModel;

    table = new QTableView;
    table->setContextMenuPolicy(Qt::CustomContextMenu);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setModel(tableModel);
    connect(table, &QTableView::customContextMenuRequested,
            this, &GeneralizedTableTab::showTableContextMenu);

    tableContextMenu = new QMenu;

    updateTableAction = new QAction("Обновить", this);
    tableContextMenu->addAction(updateTableAction);
    connect(updateTableAction, &QAction::triggered,
            this, &GeneralizedTableTab::resetQueryModel);
}


void GeneralizedTableTab::showTableContextMenu(QPoint position)
{
    QPoint formPos = table->viewport()->mapToGlobal(position);
    tableContextMenu->popup(formPos);
}


int GeneralizedTableTab::selectedID()
{
    return table->selectionModel()->currentIndex().data(Qt::UserRole).toInt();
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

    QMessageBox msgBox;
    if(role == UserRole::Unlogin) {
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
    connect(changePasswordButton, &QPushButton::clicked,
            this, &ProfileTab::changePasswordPressed);

    QSpacerItem *verticalSpacer_2 = new QSpacerItem
        (0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
    tabLayout->addItem(verticalSpacer_2);
}


void ProfileTab::logoutPressed()
{
    emit userLogout();
}


void ProfileTab::changePasswordPressed()
{
    QString old_password = oldPasswordEdit->text();
    QString new_password = newPasswordEdit->text();

    QMessageBox msgBox;
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    if(!db->checkPasswordCurrentUser(old_password)) {
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("Текущий пароль введён неверно!");
    } else if(new_password.length() < PASSWORD_MIN_LEN) {
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("Новый пароль слишком короткий!");
        QString message = "Пароль должен содержать не менее %1 символов.";
        message = message.arg(PASSWORD_MIN_LEN);
        msgBox.setInformativeText(message);
    } else if(new_password != repeatPasswordEdit->text()) {
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("Новый пароль должен совпадать в обоих полях!");
    } else if(!db->changePasswordCurrentUser(new_password)) {
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("Не удалось изменить пароль.\n"
                       "Повторите попытку позднее."
                       );
    } else {
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("Пароль успешно изменён.");
    }
    msgBox.exec();
}

//==============================================================================

DriversTab::DriversTab(DataBase *db)
    : GeneralizedTableTab(db)
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
    : GeneralizedTableTab(db)
{
    tabLayout->addWidget(table);

    LogistsTab::resetQueryModel();
}


void LogistsTab::resetQueryModel()
{
    tableModel->setQuery(db->usersQuery(UserRole::Logist));
    table->resizeColumnsToContents();
}

//==============================================================================

AccountersTab::AccountersTab(DataBase *db)
    : GeneralizedTableTab(db)
{
    tabLayout->addWidget(table);

    AccountersTab::resetQueryModel();
}


void AccountersTab::resetQueryModel()
{
    tableModel->setQuery(db->usersQuery(UserRole::Accounter));
    table->resizeColumnsToContents();
}

//==============================================================================

RoutesTab::RoutesTab(DataBase *db)
    : GeneralizedTableTab(db)
{
    tabLayout->addWidget(table);

    addRouteMenuAction = new QAction("Новый маршрут", this);
    tableContextMenu->addAction(addRouteMenuAction);
    connect(addRouteMenuAction, &QAction::triggered,
            this, &RoutesTab::addBlankRoute);

    addBasedRouteMenuAction = new QAction("Маршрут на основе выбранного", this);
    connect(addBasedRouteMenuAction, &QAction::triggered,
            this, &RoutesTab::addBasedOnSelectedRoute);

    actRouteMenuAction = new QAction("Активировать маршрут", this);
    connect(actRouteMenuAction, &QAction::triggered,
            this, &RoutesTab::activateRoute);

    deactRouteMenuAction = new QAction("Деактивировать маршрут", this);
    connect(deactRouteMenuAction, &QAction::triggered,
            this, &RoutesTab::deactivateRoute);

    RoutesTab::resetQueryModel();
}


void RoutesTab::addBlankRoute()
{
    emit requestAddBasedRoute(this, 0);
}


void RoutesTab::addBasedOnSelectedRoute()
{
    emit requestAddBasedRoute(this, selectedID());
}


void RoutesTab::activateRoute()
{
    db->activateRouteQuery(selectedID());
    resetQueryModel();
}


void RoutesTab::deactivateRoute()
{
    db->deactivateRouteQuery(selectedID());
    resetQueryModel();
}


void RoutesTab::showTableContextMenu(QPoint position)
{
    tableContextMenu->removeAction(addBasedRouteMenuAction);
    tableContextMenu->removeAction(actRouteMenuAction);
    tableContextMenu->removeAction(deactRouteMenuAction);
    QItemSelectionModel *select = table->selectionModel();
    if(select->hasSelection()) {
        tableContextMenu->addAction(addBasedRouteMenuAction);
        tableContextMenu->addAction(actRouteMenuAction);
        tableContextMenu->addAction(deactRouteMenuAction);
    }

    QPoint formPos = table->viewport()->mapToGlobal(position);
    tableContextMenu->popup(formPos);
}


void RoutesTab::resetQueryModel()
{
    tableModel->setQuery(db->routesQueryAll());
    table->resizeColumnsToContents();
}

//==============================================================================

TransportationsTab::TransportationsTab(DataBase *db)
    : GeneralizedTableTab(db)
{
    tabLayout->addWidget(table);

    addTranspMenuAction = new QAction("Добавить перевозку", this);
    tableContextMenu->addAction(addTranspMenuAction);
    connect(addTranspMenuAction, &QAction::triggered,
            this, &TransportationsTab::addBlankTransportation);

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


void TransportationsTab::addBlankTransportation()
{
    emit requestAddTransportation(this);
}


void TransportationsTab::successTransportation()
{
    db->successTranspQuery(selectedID());
    resetQueryModel();
}


void TransportationsTab::cancelTransportation()
{
    db->cancelTranspQuery(selectedID());
    resetQueryModel();
}


void TransportationsTab::reopenTransportation()
{
    db->reopenTranspQuery(selectedID());
    resetQueryModel();
}


void TransportationsTab::resetQueryModel()
{
    tableModel->setQuery(db->transpQuery());
    table->resizeColumnsToContents();
}

//==============================================================================

DriverDetailTab::DriverDetailTab(DataBase *db, int id, bool closable)
    : GeneralizedTableTab(db)
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
    */

    if(closable) {
        userEditRO = new QLineEdit;
        userEditRO->setReadOnly(true);
        userEditRO->setText(db->userFullName(userID));
        tabLayout->addRow("Водитель", userEditRO);
    } else {
        userEditRO = nullptr;
    }

    QLabel *disregardLabel =
        new QLabel("Перевозки \"В работе\" при подсчёте не учитываются!");
    tabLayout->addRow(disregardLabel);

    tabLayout->addRow(table);

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
}


void DriverDetailTab::close()
{
    deleteLater();
}

//==============================================================================

AddRouteTab::AddRouteTab(DataBase *db, int baseID)
{
    this->db = db;
    this->baseRouteID = baseID;

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

    lengthSpin = new QSpinBox();
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

    if(baseRouteID != 0) {
        fillFromBaseRoute();
    }
}


void AddRouteTab::fillFromBaseRoute()
{
    QString name;
    QString start;
    QString end;
    int length;
    QString details;
    int client_price;
    int driver_fee;

    QSqlQuery query = db->routeByIdQuery(baseRouteID);
    db->parseRoute(query, name, start, end, length, details,
                   client_price, driver_fee);

    routeNameEdit->setText(name);
    fromEdit->setText(start);
    toEdit->setText(end);
    lengthSpin->setValue(length);
    descriptionEdit->setText(details);
    priceSpin->setValue(client_price);
    driverFeeSpin->setValue(driver_fee);
}


void AddRouteTab::addRoute()
{
    QString name = routeNameEdit->text();
    QString start = fromEdit->text();
    QString end = toEdit->text();
    int length = lengthSpin->value();
    QString details = descriptionEdit->text();
    int client_price = priceSpin->value();
    int driver_fee = driverFeeSpin->value();

    QSqlQuery query;
    query = db->addRouteQuery(name, start, end, length, details,
                              client_price, driver_fee);

    emit requestUpdateTable();
    close();
}


void AddRouteTab::close()
{
    deleteLater();
}

//==============================================================================

AddTransportationTab::AddTransportationTab(DataBase *db)
{
    this->db = db;

    QFormLayout *tabLayout = new QFormLayout(this);

    driversFirstModel = new QueryModel;
    driversFirstModel->setQuery(db->usersListQuery(UserRole::Driver, false));

    driversSecondModel = new QueryModel;
    driversSecondModel->setQuery(db->usersListQuery(UserRole::Driver, true));

    routesModel = new QueryModel;
    routesModel->setQuery(db->routesListQuery());

    QSpacerItem *verticalSpacer_1 = new QSpacerItem
        (0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
    tabLayout->addItem(verticalSpacer_1);

    firstDriverCombo = new QComboBox;
    firstDriverCombo->setModel(driversFirstModel);
    tabLayout->addRow("Водитель", firstDriverCombo);

    firstDriverBonusSpin = new QSpinBox;
    tabLayout->addRow("Премия (руб)", firstDriverBonusSpin);

    QFrame *line_1 = new QFrame();
    line_1->setFrameShape(QFrame::HLine);
    tabLayout->addRow(line_1);

    secondDriverCombo = new QComboBox;
    secondDriverCombo->setModel(driversSecondModel);
    tabLayout->addRow("Второй водитель", secondDriverCombo);

    secondDriverBonusSpin = new QSpinBox;
    tabLayout->addRow("Премия (руб)", secondDriverBonusSpin);

    QFrame *line_2 = new QFrame();
    line_2->setFrameShape(QFrame::HLine);
    tabLayout->addRow(line_2);

    routeCombo = new QComboBox;
    routeCombo->setModel(routesModel);
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
    int firstID = firstDriverCombo->currentData().toInt();
    int first_bonus = firstDriverBonusSpin->value();
    int secondID = secondDriverCombo->currentData().toInt();
    int second_bonus = secondDriverBonusSpin->value();
    int routeID = routeCombo->currentData().toInt();

    QSqlQuery query;
    query = db->addTranspQuery(routeID,
                               firstID, first_bonus,
                               secondID, second_bonus);

    emit requestUpdateTable();
    close();
}


void AddTransportationTab::close()
{
    deleteLater();
}
