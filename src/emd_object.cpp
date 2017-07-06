#include "emd_object.h"

using namespace H5;
using namespace std;

void EMDObject::do_dtype(hid_t tid ){
  t_class = H5Tget_class(tid);
  t_size = H5Tget_size(tid);

  if(t_class < 0){
    //puts(" Invalid datatype.\n");
  } else {
    /*
     * Each class has specific properties that can be
     * retrieved, e.g., size, byte order, exponent, etc.
     */
    if(t_class == H5T_INTEGER) {
      int_sign = H5Tget_sign(tid);
      /* display size, signed, endianess, etc. */
    } else if(t_class == H5T_FLOAT) {
      /* display size, endianess, exponennt, etc. */
    } else if(t_class == H5T_STRING) {
      /* display size, padding, termination, etc. */
    } else if(t_class == H5T_BITFIELD) {
      //puts(" Datatype is 'H5T_BITFIELD'.\n");
      /* display size, label, etc. */
    } else if(t_class == H5T_OPAQUE) {
      /* display size, etc. */
    } else if(t_class == H5T_COMPOUND) {
      /* recursively display each member: field name, type  */
    } else if(t_class == H5T_ARRAY) {
      /* display  dimensions, base type  */
    } else if(t_class == H5T_ENUM) {
      /* display elements: name, value   */
    } else  {
      /* eg. Object Reference, ...and so on ... */
    }
  }
}

void EMDObject::do_link(hid_t gid, char *name){
  herr_t status;
  char target[MAX_NAME];
  status = H5Gget_linkval(gid, name, MAX_NAME, target  ) ;
}
