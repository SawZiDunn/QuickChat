#include "mainwindow.h"
#include "setup_db.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    setup_chat_db();
    MainWindow w;

    w.show();
    return a.exec();
}
