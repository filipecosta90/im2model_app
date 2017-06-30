/* BEGIN BOOST */
#include <boost/filesystem/operations.hpp>                // for directory_iterator
#include <boost/filesystem/path.hpp>                      // for path, operator==, oper...
/* END BOOST */


#include "configwin.h"
#include <iostream>
#include <QtWidgets>
#include <QTreeView>
#include <QtCore>
#include <QApplication>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QAction>

#include "application_log.hpp"
#include "QtAwesome.h"


int main(int argc, char *argv[]){
  std::cout <<  QDir::currentPath().toStdString() << std::endl;

  // to access resources
  Q_INIT_RESOURCE(im2model);
  QCoreApplication::addLibraryPath("./");

  QApplication app(argc, argv);
  app.setOrganizationName("uminho");
  app.setApplicationName("Im2Model");

  std::cout <<  QCoreApplication::applicationDirPath().toStdString() << std::endl;
  std::cout <<  QDir::currentPath().toStdString() << std::endl;

  boost::filesystem::path full_path( boost::filesystem::current_path() );
  boost::filesystem::path app_path( QCoreApplication::applicationDirPath().toStdString() );

  ApplicationLog::ApplicationLog* im2model_logger = new ApplicationLog::ApplicationLog( app_path );
  im2model_logger->logEvent(ApplicationLog::notification, "Application start");
  std::stringstream message;
  message << "applicationDirPath: " << app_path.string();
  im2model_logger->logEvent( ApplicationLog::notification, message.str() );
  message = std::stringstream();
  message << "fullPath: " <<  full_path.string();
  im2model_logger->logEvent( ApplicationLog::notification, message.str() );

  // to ease the load process
  QCommandLineParser parser;
  parser.setApplicationDescription(QCoreApplication::applicationName());
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addPositionalArgument("file", "The file to open.");
  parser.process(app);
  MainWindow window( im2model_logger );

  if( window._is_initialization_ok() ){
    if (!parser.positionalArguments().isEmpty()){
      window.loadFile(parser.positionalArguments().first());
    }
    //const bool base_setted = window.set_base_dir_path( app_path );
    //if( base_setted ){
      window.show();
      return app.exec();
    //}
  }
  im2model_logger->logEvent(ApplicationLog::normal, "Application exit");

  return 0;
}
