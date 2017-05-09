#include "configwin.h"
#include <QApplication>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QTreeView>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    MainWindow w;
    w.show();

    return a.exec();



}
