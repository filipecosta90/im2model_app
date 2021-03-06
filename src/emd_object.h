/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

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

#define H5_BUILT_AS_DYNAMIC_LIB
#include <H5Cpp.h>

#include "../include/rapidjson/reader.h"
#include "../include/rapidjson/stream.h"
#include "../include/rapidjson/document.h"

#define MAX_NAME 1024

using namespace H5;
using namespace std;

// based on https://support.hdfgroup.org/ftp/HDF5/examples/misc-examples/h5_info.c

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

    /*
        H5T_NATIVE_CHAR         
        H5T_NATIVE_SHORT        
        H5T_NATIVE_INT          
        H5T_NATIVE_LONG         
        H5T_NATIVE_LLONG        

        H5T_NATIVE_UCHAR
        H5T_NATIVE_USHORT
        H5T_NATIVE_UINT
        H5T_NATIVE_ULONG
        H5T_NATIVE_ULLONG

        H5T_NATIVE_FLOAT
        H5T_NATIVE_DOUBLE
        H5T_NATIVE_LDOUBLE

        H5T_NATIVE_B8
        H5T_NATIVE_B16
        H5T_NATIVE_B32
        H5T_NATIVE_B64
    */
    hid_t native_dtype;

    // datatype
    H5T_class_t t_class;
    // size of datatype
    size_t t_size;
    // Integer data can be signed two's complement (H5T_SGN_2) or unsigned (H5T_SGN_NONE)
    H5T_sign_t int_sign;

public:
    std::string get_name(){ return name; }
    H5T_sign_t get_H5T_sign_t(){ return int_sign; }
    H5T_class_t get_H5T_class_t(){ return t_class; }
    size_t get_H5T_class_t_size_t(){ return t_size; }
    hid_t get_native_dtype(){ return native_dtype; }

    bool is_signed_int();
    bool is_numeric();

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
