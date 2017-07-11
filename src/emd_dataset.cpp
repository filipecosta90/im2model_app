#include "emd_dataset.h"

EMDDataSet::EMDDataSet( hid_t id ){
  dsid = id;
}

const std::vector<unsigned char>& EMDDataSet::get_raw_data() const {
  return raw_data;
}

hsize_t* EMDDataSet::get_chunk_dims_out(){
  return chunk_dims_out;
}

/*
 *  Run through all the attributes of a dataset or group.
 *  This is similar to iterating through a group.
 */

void EMDDataSet::scan_attrs(hid_t oid) {
  int na;
  hid_t aid;
  int i;

  na = H5Aget_num_attrs(oid);

  for (i = 0; i < na; i++) {
    aid =	H5Aopen_idx(oid, (unsigned int)i );
    EMDAttribute* new_attribute = new EMDAttribute( aid );
    new_attribute->do_attr( aid );
    _attributes.push_back( new_attribute );
    H5Aclose(aid);
  }
}

/*
 *  Retrieve information about a dataset.
 *
 *  Many other possible actions.
 *
 *  This example does not read the data of the dataset.
 */
void EMDDataSet::do_dset(hid_t did){
  hid_t tid;
  hid_t pid;
  // dataset space
  hid_t space_id;
  hsize_t size;
  char ds_name[MAX_NAME];
  /*
   * Information about the group:
   *  Name and attributes
   *
   *  Other info., not shown here: number of links, object id
   */
  H5Iget_name(did, ds_name, MAX_NAME  );
  name = ds_name;

  /*
   *  process the attributes of the dataset, if any.
   */
  scan_attrs(did);
  /*
   * Get dataset information: dataspace, data type
   */
  space_id = H5Dget_space(did); /* the dimensions of the dataset (not shown) */
  tid = H5Dget_type(did);
  do_dtype(tid);

  /*
   * Retrieve and analyse the dataset properties
   */
  pid = H5Dget_create_plist(did); /* get creation property list */
  do_plist(pid);
  size = H5Dget_storage_size(did);

  size_raw_data = chunk_dims_out[0];
  for(size_t pos_rank = 1; pos_rank < rank_chunk; pos_rank++ ){
    size_raw_data *= chunk_dims_out[pos_rank];
  }

  herr_t      status;
  /*
   * Create a datatype to refer to.
   */
  hid_t		type_id;       /* Datatype ID	   	 */
  hid_t		size1;       /* Datatype ID	   	 */

  type_id = H5Tvlen_create (H5T_NATIVE_USHORT);
  size1 = H5Dget_storage_size(did);
  raw_data.resize(static_cast<int>(size1), 0x00); //Allocate and Zero the array
  type_id = H5Dget_type(did);
  status = H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, &(raw_data.front()) );

  /*
   * The datatype and dataspace can be used to read all or
   * part of the data.  (Not shown in this example.)
   */

  /* ... read data with H5Dread, write with H5Dwrite, etc. */
  H5Pclose(pid);
  H5Tclose(tid);
  H5Sclose(space_id);
}


/*
 *   Example of information that can be read from a Dataset Creation
 *   Property List.
 *
 *   There are many other possibilities, and there are other property
 *   lists.
 */
void EMDDataSet::do_plist(hid_t pid) {

  H5Z_filter_t  filtn;
  int i;
  unsigned int   filt_flags, filt_conf;
  size_t cd_nelmts;
  unsigned int cd_values[32] ;
  char f_name[MAX_NAME];
  H5D_fill_time_t ft;
  H5D_alloc_time_t at;
  H5D_fill_value_t fvstatus;
  unsigned int szip_options_mask;
  unsigned int szip_pixels_per_block;

  /* zillions of things might be on the plist */
  /*  here are a few... */

  /*
   * get chunking information: rank and dimensions.
   *
   *  For other layouts, would get the relevant information.
   */
  if(H5D_CHUNKED == H5Pget_layout(pid)){
    rank_chunk = H5Pget_chunk(pid, 3, chunk_dims_out);
  } /* else if contiguous, etc. */

  /*
   *  Get optional filters, if any.
   *
   *  This include optional checksum and compression methods.
   */

  nfilters = H5Pget_nfilters(pid);
  for (i = 0; i < nfilters; i++)
  {
    /* For each filter, get
     *   filter ID
     *   filter specific parameters
     */
    cd_nelmts = 32;
    filtn = H5Pget_filter(pid, (unsigned)i,
        &filt_flags, &cd_nelmts, cd_values,
        (size_t)MAX_NAME, f_name, &filt_conf);
    /*
     *  These are the predefined filters
     */
    switch (filtn) {
      case H5Z_FILTER_DEFLATE:  /* AKA GZIP compression */
        break;
      case H5Z_FILTER_SHUFFLE:
        break;
      case H5Z_FILTER_FLETCHER32:
        break;
      case H5Z_FILTER_SZIP:
        szip_options_mask=cd_values[0];;
        szip_pixels_per_block=cd_values[1];

        /* print SZIP options mask, etc. */
        break;
      default:
        break;
    }
  }

  /*
   *  Get the fill value information:
   *    - when to allocate space on disk
   *    - when to fill on disk
   *    - value to fill, if any
   */

  H5Pget_alloc_time(pid, &at);

  switch (at)
  {
    case H5D_ALLOC_TIME_EARLY:
      break;
    case H5D_ALLOC_TIME_INCR:
      break;
    case H5D_ALLOC_TIME_LATE:
      break;
    default:
      break;
  }

  H5Pget_fill_time(pid, &ft);
  switch ( ft )
  {
    case H5D_FILL_TIME_ALLOC:
      break;
    case H5D_FILL_TIME_NEVER:
      break;
    case H5D_FILL_TIME_IFSET:
      break;
    default:
      break;
  }

  H5Pfill_value_defined(pid, &fvstatus);

  if (fvstatus == H5D_FILL_VALUE_UNDEFINED)
  {
  } else {
    /* Read  the fill value with H5Pget_fill_value.
     * Fill value is the same data type as the dataset.
     * (details not shown)
     **/
  }

  /* ... and so on for other dataset properties ... */
}
