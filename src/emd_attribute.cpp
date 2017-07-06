#include "emd_attribute.h"

using namespace H5;
using namespace std;

EMDAttribute::EMDAttribute( hid_t id ){
  aid = id;
}

void EMDAttribute::do_attr(hid_t aid ){

  char buf[MAX_NAME];
  /*
   * Get the name of the attribute.
   */
  len = H5Aget_name(aid, MAX_NAME, buf );
  name = buf;

  /*
   * Get attribute information: dataspace, data type
   */
  aspace = H5Aget_space(aid); /* the dimensions of the attribute data */
  atype  = H5Aget_type(aid);
  do_dtype(atype);

  /*
   * The datatype and dataspace can be used to read all or
   * part of the data.  (Not shown in this example.)
   */

  /* ... read data with H5Aread, write with H5Awrite, etc. */
  H5Tclose(atype);
  H5Sclose(aspace);
}
