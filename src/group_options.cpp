#include "group_options.h"

group_options::group_options( std::string gname ){
  _group_name = gname;
}

bool group_options::is_update_required(){
    bool result = false;
    for(auto const &_curr_var : _current_variables_map ) {
        const time_t _curr_var_time = _curr_var.second;
      if( _curr_var_time > _runned_time ){
        result = true;
        emit update_required( _group_name );
      }
    }
    return result;
}

bool group_options::add_option( std::string varname, std::string description, bool required ){
  bool result = false;
  if(  variables_map.find( varname ) == variables_map.end() ){
    variables_map.insert(std::pair<std::string,std::string>(varname,description));
    _required_variables_map.insert(std::pair<std::string,bool>(varname,required));
    result = true;
  }
  return result;
}

bool group_options::add_option( TreeItem* item , bool required ){
  std::string varname = item->get_variable_name();
  std::string description = item->get_variable_name();
  bool result = false;
  if(  variables_map.find( varname ) == variables_map.end() ){
    std::cout << "tracking a new variable " << varname << " [required: " << required << " ] in group options: " << _group_name << std::endl;
    connect( item, SIGNAL(dataChanged( std::string )), this, SLOT( update_track_var( std::string ) ) );
    variables_map.insert(std::pair<std::string,std::string>(varname,description));
    _required_variables_map.insert(std::pair<std::string,bool>(varname,required));
    result = true;
  }
  return result;
}

void group_options::listen_group_update_required( group_options* group_to_listen ){
    _groups_to_listen.push_back( group_to_listen );
    connect( group_to_listen, SIGNAL(update_required( std::string )), this, SLOT( set_update_required_from_pipeline( std::string ) ) );
}

void group_options::set_update_required_from_pipeline( std::string signaling_group_name ){
    std::cout << "group dependency from "<< signaling_group_name << " is forcing an update required on group " << _group_name << std::endl;
    _force_update_from_group_dependency = true;
    emit update_required( _group_name );
}

bool group_options::update_track_var( std::string varname ){
  std::cout << " private slot called update_track_var on var: " << varname << std::endl;
  bool result = false;
  // if var exists both in the variable map and in the current variable map
  if(  variables_map.find( varname ) != variables_map.end() ){
    time_t TheTime; // declare a time object
    time(&TheTime); // Get the current time
    if( _current_variables_map.find( varname ) == _current_variables_map.end() ){
      _current_variables_map.insert(std::pair<std::string,time_t>(varname,TheTime));
      if( _flag_runned_time ){
          if( TheTime  > _runned_time ){
              emit update_required( _group_name );
          }
      }
      std::cout << "\tnew tracking" << std::endl;
    }
    else{
      _current_variables_map[varname] = TheTime;
      std::cout << "\tupdating tracking" << std::endl;

    }
    result = true;
  }
  return result;
}

bool group_options::are_group_vars_setted_up( ){
    bool result = true;
    // pipeline dependecies
    for( auto const &_group : _groups_to_listen ){
        result &= _group->are_group_vars_setted_up();
    }
    if( result ){
  // vars dependencies
  for(auto const &_full_var : _required_variables_map ) {
    const bool _required = _full_var.second;
    if( _required ){
      const std::string varname = _full_var.first;
      bool found = false;
      if( _current_variables_map.find( varname ) != _current_variables_map.end() ){
        found = true;
      }
      result &= found;
    }
  }
    }
  return result;
}

void group_options::mark_runned_time(){
  time_t TheTime; // declare a time object
  time(&TheTime); // Get the current time
  _runned_time = TheTime;
  _flag_runned_time = true;
  _force_update_from_group_dependency = false;
}

time_t group_options::get_runned_time(){
  return _runned_time;
}