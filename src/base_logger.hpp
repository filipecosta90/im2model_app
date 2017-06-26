#ifndef SRC_BASELOGGER_H__
#define SRC_BASELOGGER_H__

/* BEGIN BOOST */
#include <boost/process.hpp>
#include <boost/filesystem/operations.hpp>                // for directory_iterator
#include <boost/filesystem/path.hpp>                      // for path, operator==, oper...
#include <boost/iterator/iterator_facade.hpp>             // for iterator_facade_base
#include <boost/thread.hpp>                               // for thread
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
/* END BOOST */

#include <iosfwd>                    // for string
#include <iostream>

#include <opencv2/core/hal/interface.h>                   // for CV_8UC1
#include <opencv2/imgcodecs/imgcodecs_c.h>                // for ::CV_LOAD_I...
#include <opencv2/core.hpp>                               // for RNG
#include <opencv2/core/cvstd.inl.hpp>                     // for String::String
#include <opencv2/core/mat.hpp>                           // for Mat
#include <opencv2/core/mat.inl.hpp>                       // for Mat::Mat
#include <opencv2/core/operations.hpp>                    // for RNG::RNG
#include <opencv2/core/types.hpp>                         // for Rect, Point3d
#include <opencv2/imgcodecs.hpp>                          // for imread

#include "application_log.hpp"

class BaseLogger {
  private:


  protected:
    /* Loggers */
    ApplicationLog::ApplicationLog* logger = nullptr;
    bool _flag_logger = false;

  public:
    BaseLogger();
    /** getters **/
    // flag getters
    bool get_flag_logger(){ return _flag_logger; }

    /* Loggers */
    ApplicationLog::ApplicationLog* get_logger(){ return logger; }

    /** setters **/
    /* Loggers */
    bool set_application_logger( ApplicationLog::ApplicationLog* logger );
};

#endif
