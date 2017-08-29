#include "application_log.hpp"

namespace ApplicationLog{

  BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", severity_level)
  BOOST_LOG_ATTRIBUTE_KEYWORD(id, "ID", unsigned int)

  boost::log::attributes::mutable_constant<double> attrSetPoint(0);
  boost::log::attributes::constant<int> attrID(1);


  ApplicationLog::ApplicationLog( boost::filesystem::path app_path ){

    // Access the core
    boost::shared_ptr<boost::log::core> core = boost::log::core::get();

    // check to see if the destination folder exists
    boost::filesystem::path targetFolder = app_path / boost::filesystem::path("log") ;

    // if not, create it
    if (!boost::filesystem::exists(targetFolder) || !boost::filesystem::is_directory(targetFolder))
    {
      boost::filesystem::create_directory(targetFolder);
    }

    // Create the log file name in the form ./logs/com.uminho.im2model.log
    boost::filesystem::path file ( "com.uminho.im2model_%Y%m%d_%H%M%S_%5N.log");
    full_path_file = targetFolder / file;

    // Create a text file sink
    typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend> file_sink;


    // Automatically rotate files when they reach 2 mb
    boost::shared_ptr<file_sink> sink(new file_sink(
          // the resulting file name pattern
          keywords::file_name = full_path_file.string(),
          // rotation size, in characters
          keywords::rotation_size =  2 * 1024 * 1024
          ));

    // Set up where the rotated files will be stored
    sink->locked_backend()->set_file_collector(
        sinks::file::make_collector(
          boost::log::keywords::target = targetFolder,         // the target directory
          boost::log::keywords::max_size = 2 * 1024 * 1024    // rotate files after 2 megabytes
          ));

    // Upon restart, scan the directory for files matching the file_name pattern and delete any required files
    sink->locked_backend()->scan_for_files();

    // Enable auto-flushing after each log record written
    sink->locked_backend()->auto_flush(true);

    // add common attributes like date and time.
    boost::log::add_common_attributes();
    boost::log::core::get()->add_global_attribute("Scope",  boost::log::attributes::named_scope());

    // add an ID attribute. This is how the core will know to write only to
    // this log file, as set by the filter
    auto attr = attrs::constant<unsigned int>(99);
    m_logger.add_attribute("ID", attr);

    sink->set_filter( expr::attr<unsigned int>("ID") == 99 );

    auto fmtTimeStamp = boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f");

    auto fmtThreadId = boost::log::expressions::attr<boost::log::attributes::current_thread_id::value_type>("ThreadID");

    auto fmtLineN = boost::log::expressions::attr< unsigned int >("LineID");

    auto fmtScope = boost::log::expressions::format_named_scope("Scope",
            boost::log::keywords::format = "%n(%f:%l)",
            boost::log::keywords::iteration = boost::log::expressions::reverse,
            boost::log::keywords::depth = 3);

            boost::log::formatter logFmt =
       boost::log::expressions::format("%1%\t(%2%)\tThread: %3% \tSeverity: %4% \n\t\t\tScope: %5% \n\t\t\tMessage: %6%\n")
       % fmtLineN % fmtTimeStamp % fmtThreadId % severity % fmtScope % boost::log::expressions::smessage;
        sink->set_formatter( logFmt );

    // Add the sink to the core
    core->add_sink(sink);
  }

      // Actually designate the message to the log
      void ApplicationLog::logEvent(const severity_level level, std::string message){
          BOOST_LOG_SEV(m_logger, level)  << message;
        }

    // Actually designate the message to the log
    void ApplicationLog::logEvent(const severity_level level, std::string message, std::string file, std::string line, std::string function){
        BOOST_LOG_SEV(m_logger, level)  << "(" << file << ":" << line << ":" << function << ") " << message;
      }
}
