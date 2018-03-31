/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#ifndef __APP_LOG_H__
#define __APP_LOG_H__

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>                // for directory_iterator
#include <boost/filesystem/path.hpp>                      // for path, operator==, oper...
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/filesystem.hpp>

#define LOG_EVENT( level, message )	ApplicationLog::logEvent(level, message, __FILE__, __LINE__, __FUNCTION__ )

namespace ApplicationLog {
  namespace logging = boost::log;
  namespace expr = boost::log::expressions;
  namespace sinks = boost::log::sinks;
  namespace attrs = boost::log::attributes;
  namespace src = boost::log::sources;
  namespace keywords = boost::log::keywords;

  enum severity_level {
    normal,
    notification,
    warning,
    error,
    critical
  };

  class ApplicationLog {
    public:
      ApplicationLog( boost::filesystem::path full_path_file );
      void logEvent(const severity_level level, std::string message);
      void logEvent(const severity_level level, std::string message, std::string file , std::string line, std::string function);
      void add_sync( boost::filesystem::path  new_path_file  );
    private:
      src::severity_logger<severity_level> m_logger;
      boost::filesystem::path full_path_file;
  };

}

#endif
