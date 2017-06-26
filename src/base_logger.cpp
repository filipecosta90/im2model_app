#include "base_logger.hpp"

BaseLogger::BaseLogger() {
}

/* Loggers */
bool BaseLogger::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
  logger->logEvent( ApplicationLog::notification, "Application logger setted for BaseLogger class." );
  return true;
}
