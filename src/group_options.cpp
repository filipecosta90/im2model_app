#include "group_options.h"

group_options::group_options( std::string gname ){
 _group_name = gname;
}

bool group_options::is_update_required( ){
    return false;
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

bool group_options::update_track_var( std::string varname ){
    std::cout << " private slot called update_track_var on var: " << varname << std::endl;
    bool result = false;
    // if var exists both in the variable map and in the current variable map
    if(  variables_map.find( varname ) != variables_map.end() ){
        time_t TheTime; // declare a time object
        time(&TheTime); // Get the current time
        if( _current_variables_map.find( varname ) == _current_variables_map.end() ){
            _current_variables_map.insert(std::pair<std::string,time_t>(varname,TheTime));
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

bool group_options::is_group_ready(){
    return true;
}
