#ifndef SRC_EMDATTRIBUTE_H__
#define SRC_EMDATTRIBUTE_H__

#include <iosfwd>                    // for string
#include <iostream>

#include "H5Cpp.h"
#include "hdf5.h"

#include "emd_object.h"

using namespace H5;
using namespace std;


class EMDAttribute : public EMDObject {

protected:
    hid_t aid;
ssize_t len;
hid_t atype;
hid_t aspace;
public:
EMDAttribute( hid_t id );

/*
 *  Process one attribute.
 *  This is similar to the information about a dataset.
 */
void do_attr(hid_t aid);

};

#endif
