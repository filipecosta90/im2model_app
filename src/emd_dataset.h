#ifndef SRC_EMDDATASET_H__
#define SRC_EMDDATASET_H__

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

#include "../include/rapidjson/reader.h"
#include "../include/rapidjson/stream.h"
#include "../include/rapidjson/document.h"

#include "emd_object.h"
#include "emd_attribute.h"


#define MAX_NAME 1024

using namespace H5;
using namespace std;

class EMDDataSet : public EMDObject {
  private:
  protected:
    std::vector<EMDAttribute*> _attributes;

    // size of chunks for the raw data of a chunked layout dataset
    hsize_t chunk_dims_out[3];
    int  rank_chunk;
    int nfilters;

    size_t size_raw_data;

    std::vector<unsigned char> raw_data;

  public:
    EMDDataSet( hid_t id );
    const std::vector<unsigned char>&  get_raw_data() const;

    hsize_t* get_chunk_dims_out();

    /*
     *  Run through all the attributes of a dataset or group.
     *  This is similar to iterating through a group.
     */
    void scan_attrs(hid_t oid);

    /*
     *  Retrieve information about a dataset.
     *  Many other possible actions.
     *  This example does not read the data of the dataset.
     */
    void do_dset(hid_t did);

    /*
     *   Example of information that can be read from a Dataset Creation
     *   Property List.
     *   There are many other possibilities, and there are other property
     *   lists.
     */
    void do_plist(hid_t pid);

};

#endif
