/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#include "emd_object.h"

using namespace H5;
using namespace std;

void EMDObject::do_dtype(hid_t tid ){
  t_class = H5Tget_class(tid);
  t_size = H5Tget_size(tid);

  //Returns the native datatype of a specified datatype.

  if(t_class < 0){
    //puts(" Invalid datatype.\n");
  } else {
    /*
     * Each class has specific properties that can be
     * retrieved, e.g., size, byte order, exponent, etc.
     */
    native_dtype = H5Tget_native_type(tid, H5T_DIR_DESCEND );

    if( t_class == H5T_INTEGER ) {
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

bool EMDObject::is_numeric(){
   bool result = false;
  if( ( t_class == H5T_INTEGER ) || (t_class == H5T_FLOAT) ) {
      result = true;
    }
  return result;
}

bool EMDObject::is_signed_int(){
  bool result = false;
  if( t_class == H5T_INTEGER ) {
      result = ( int_sign == H5T_SGN_2 ) ? true : false;
    }
  return result;
}

void EMDObject::do_link(hid_t gid, char *name){
  herr_t status;
  char target[MAX_NAME];
  status = H5Gget_linkval(gid, name, MAX_NAME, target  ) ;
}
