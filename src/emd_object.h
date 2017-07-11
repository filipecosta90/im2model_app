#ifndef SRC_EMDOBJECT_H__
#define SRC_EMDOBJECT_H__

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

#include "H5Cpp.h"
#include "hdf5.h"

#include "rapidjson/reader.h"
#include "rapidjson/stream.h"
#include "rapidjson/document.h"

#define MAX_NAME 1024

using namespace H5;
using namespace std;

class EMDObject {
  private:

  protected:
    std::string name;
    int i;
    ssize_t len;
    hsize_t nobj;
    herr_t err;
    int otype;
    hid_t type_id;
    hid_t dsid;

    // datatype
    H5T_class_t t_class;
    // size of datatype
    size_t t_size;
    // Integer data can be signed two's complement (H5T_SGN_2) or unsigned (H5T_SGN_NONE)
    H5T_sign_t int_sign;

  public:
    std::string get_name(){ return name; }

    /*
     *  Analyze a data type description
     */
    void do_dtype(hid_t tid);

    /*
     *  Analyze a symbolic link
     *
     * The main thing you can do with a link is find out
     * what it points to.
     */
    void do_link(hid_t gid, char *name);
};

#endif