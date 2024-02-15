#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.setWindowTitle("GDB Client");
    w.setWindowIcon(QIcon(":/icn.ico"));


    w.show();
    return a.exec();
}
