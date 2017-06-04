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

    private:
      src::severity_logger<severity_level> m_logger;
      boost::filesystem::path full_path_file;
  };

}

#endif
