/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#ifndef SRC_EMDATTRIBUTE_H__
#define SRC_EMDATTRIBUTE_H__

#include <iosfwd>                    // for string
#include <iostream>

#define H5_BUILT_AS_DYNAMIC_LIB
#include <H5Cpp.h>

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
