/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

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
