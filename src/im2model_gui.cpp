#include <QtWidgets>
#include <QTreeView>
#include "configwin.h"


#include <QApplication>
#include <QFileDialog>
#include <QFileSystemModel>



int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  MainWindow w;
  w.show();

  return a.exec();
}
