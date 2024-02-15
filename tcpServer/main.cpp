#include "mainwindow.h"

#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("St-link GDB Server");
    w.setWindowIcon(QIcon(":/icn.ico"));

    w.show();
    return a.exec();
}
