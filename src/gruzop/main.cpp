#include "mainwindow.h"
#include "database.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DataBase db;
    db.reconnect("gestart", "");
    MainWindow w(nullptr, &db);
    w.show();
    return a.exec();
}
