#include "emd_group.h"

EMDGroup::EMDGroup( ){
}

bool EMDGroup::get_flag_contains_group( std::string name ){
  bool result = false;
  std::map<std::string,EMDGroup*>::iterator it;
  it = _map_groups.find( name );
  if (it != _map_groups.end()){
    result = true;
  }
  return result;
}

EMDGroup* EMDGroup::get_group( std::string name ){
  EMDGroup* ret_group;
  std::map<std::string,EMDGroup*>::iterator it;
  it = _map_groups.find( name );
  if (it != _map_groups.end()){
    ret_group = it->second;
  }
  return ret_group;
}

EMDGroup* EMDGroup::get_child_group( int child_pos  ){
  EMDGroup* ret_group = nullptr;
  if(_groups.size() > child_pos ){
    ret_group = _groups.at(child_pos);
  }
  return ret_group;
}


bool EMDGroup::get_flag_contains_dataset( std::string dataset_name ){
  bool result = false;
  std::map<std::string,EMDDataSet*>::iterator it;
  it = _map_datasets.find( dataset_name );
  if (it != _map_datasets.end()){
    result = true;
  }
  return result;
}

EMDDataSet* EMDGroup::get_dataset( std::string dataset_name ){
  EMDDataSet* ret_dataset = nullptr;
  std::map<std::string,EMDDataSet*>::iterator it;
  it = _map_datasets.find( dataset_name );
  if (it != _map_datasets.end()){
    ret_dataset = it->second ;
  }
  return ret_dataset;
}

/*
 *  Run through all the attributes of a dataset or group.
 *  This is similar to iterating through a group.
 */
void EMDGroup::scan_attrs(hid_t oid) {
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
 * Process a group and all it's members
 *
 *   This can be used as a model to implement different actions and
 *   searches.
 */
void EMDGroup::scan_group(hid_t grp_id ) {
gid = grp_id;
  /*
   * Information about the group:
   *  Name and attributes
   *
   *  Other info., not shown here: number of links, object id
   */
  len = H5Iget_name (gid, group_name, MAX_NAME);
  name = group_name;

  /*
   *  process the attributes of the group, if any.
   */
  scan_attrs(gid);

  /*
   *  Get all the members of the groups, one at a time.
   */
  err = H5Gget_num_objs(gid, &nobj);
  for (i = 0; i < nobj; i++) {
    /*
     *  For each object in the group, get the name and
     *   what type of object it is.
     */
    //printf("  Member #: %d ",i);fflush(stdout);
    len = H5Gget_objname_by_idx(gid, (hsize_t)i, memb_name, (size_t)MAX_NAME );

    otype = H5Gget_objtype_by_idx(gid, (size_t)i );
    /*
     * process each object according to its type
     */
    switch(otype) {
      case H5G_LINK:
        {
          do_link(gid,memb_name);
          break;
        }
      case H5G_GROUP:
        {
          grpid = H5Gopen(gid,memb_name, H5P_DEFAULT);
          EMDGroup* new_group = new EMDGroup( );
          new_group->scan_group( grpid );
          std::string g_name = new_group->get_name();
          std::map<std::string,EMDGroup*>::iterator it = _map_groups.begin();
          _map_groups.insert(it, std::pair<std::string,EMDGroup*>(g_name,new_group));  // max efficiency inserting
          _groups.push_back( new_group );
          H5Gclose(grpid);
          break;
        }
      case H5G_DATASET:
        {
          dsid = H5Dopen(gid,memb_name, H5P_DEFAULT);
          EMDDataSet* new_dataset = new EMDDataSet( dsid );
          new_dataset->do_dset( dsid );
          std::string dataset_name = new_dataset->get_name();
          std::map<std::string,EMDDataSet*>::iterator it = _map_datasets.begin();
          _map_datasets.insert(it, std::pair<std::string,EMDDataSet*>(dataset_name,new_dataset));  // max efficiency inserting
          _datasets.push_back( new_dataset );
          H5Dclose(dsid);
          break;
        }
      case H5G_TYPE:
        {
          type_id = H5Topen(gid,memb_name, H5P_DEFAULT);
          do_dtype(type_id);
          H5Tclose(type_id);
          break;
        }
      default:
        {
          break;
        }
    }
  }
}
