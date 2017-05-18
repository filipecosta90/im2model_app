#include <QtWidgets>
#include <QTreeView>
#include "configwin.h"

#include <QtCore>
#include <iostream>

#include <QApplication>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QAction>


int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  MainWindow window;
  window.show();
  return app.exec();
}
