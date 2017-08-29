#include "base_logger.hpp"

BaseLogger::BaseLogger() {
}

/* Loggers */
bool BaseLogger::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
 BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification, "Application logger setted for BaseLogger class." );
  return true;
}
