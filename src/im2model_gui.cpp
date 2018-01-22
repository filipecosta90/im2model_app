/* BEGIN BOOST */
#include <boost/filesystem/operations.hpp>                // for directory_iterator
#include <boost/filesystem/path.hpp>                      // for path, operator==, oper...
/* END BOOST */

#include "application_log.hpp"
#include "configwin.h"
#include "QtAwesome.h"
#include "version_config.h"

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
#include <QEvent>
#include <QObject>
#include <QApplication>


class Application final : public QApplication {
  public:
    ApplicationLog::ApplicationLog* im2model_logger;

    Application(int& argc, char** argv ) : QApplication(argc, argv) {
      QCoreApplication::addLibraryPath("./");
      boost::filesystem::path full_path( boost::filesystem::current_path() );
      boost::filesystem::path app_path( QCoreApplication::applicationDirPath().toStdString() );
      im2model_logger = new ApplicationLog::ApplicationLog( app_path );
      BOOST_LOG_FUNCTION();
      im2model_logger->logEvent(ApplicationLog::notification, "Application start");
      std::stringstream message;
      message << "applicationDirPath: " << app_path.string();
      im2model_logger->logEvent( ApplicationLog::notification, message.str() );
      message = std::stringstream();
      message << "fullPath: " <<  full_path.string();
      im2model_logger->logEvent( ApplicationLog::notification, message.str() );
    }

    bool notify(QObject *receiver, QEvent *event) override {
      BOOST_LOG_FUNCTION();
      bool result = false;
      try {
        result = QApplication::notify(receiver, event);
      } catch (boost::exception &ex) {
        QMessageBox msgBox;
        msgBox.setText("Caught an unhandled exception.");
        QString message = "Caught an unhandled exception\"";
        message += boost::diagnostic_information(ex).c_str();
        message += "\" sending event ";
        message += typeid(*event).name();
        message += " to object ";
        message += qPrintable(receiver->objectName());
        message += "\nWill try to recover from it.";
        im2model_logger->logEvent( ApplicationLog::error, message.toStdString() );
        msgBox.setInformativeText( message );
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
      } catch (std::exception &ex) {
        QMessageBox msgBox;
        msgBox.setText("Caught an unhandled execption.");
        QString message = "Caught an unhandled execption\"";
        message += ex.what();
        message += "\" sending event ";
        message += typeid(*event).name();
        message += " to object ";
        message += qPrintable(receiver->objectName());
        message += "\nWill try to recover from it.";
        im2model_logger->logEvent( ApplicationLog::error, message.toStdString() );
        msgBox.setInformativeText( message );
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
      }
      return result;
    }
};


int main(int argc, char *argv[]){
  qRegisterMetaType<std::string>("std::string");
  
  std::cout <<  QDir::currentPath().toStdString() << std::endl;

  // to access resources
  Q_INIT_RESOURCE(im2model);
QDir::setSearchPaths("qrc", QStringList(":/") );

  Application app(argc, argv );
    app.setOrganizationName("uminho");
    app.setApplicationName("Im2Model");

 std::cout << "Im2Model version: " << getIm2ModelVersion() << std::endl;
  // to ease the load process
  QCommandLineParser parser;
  parser.setApplicationDescription(QCoreApplication::applicationName());
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addPositionalArgument("file", "The file to open.");
  parser.process(app);
  MainWindow window( app.im2model_logger );
  window.setApplicationVersion( getIm2ModelVersion() );

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
  app.im2model_logger->logEvent(ApplicationLog::normal, "Application exit");

  return 0;
}
