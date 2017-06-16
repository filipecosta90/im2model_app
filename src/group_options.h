#ifndef GROUP_OPTIONS_H
#define GROUP_OPTIONS_H

#include <QMainWindow>
#include <QtWidgets>
#include <QObject>

#include "treeitem.h"
#include <iostream>
#include <ctime>

class group_options : public QObject {
    Q_OBJECT
private:
    // key varname, value description
    std::map<std::string,std::string> variables_map;
    // key varname, value = required or no
    std::map<std::string,bool> _required_variables_map;
    // key varnname, value = time of last update
    std::map<std::string,time_t> _current_variables_map;
    // options name
    std::string _group_name;

    time_t _runned_time;
    bool _flag_runned_time = false;
    bool _force_update_from_group_dependency;

private slots:
    bool update_track_var( std::string varname );
    void set_update_required_from_pipeline( std::string signaling_group_name );

public:
void listen_group_update_required( group_options* group_to_listen );
    group_options( std::string _group_name );
    bool is_update_required();
    bool add_option( std::string varname, std::string description, bool required );
    bool add_option( TreeItem* item, bool required );
    bool are_group_vars_setted_up();
    void mark_runned_time();
    time_t get_runned_time();

signals:
    void update_required( std::string _group_name );
};

#endif // GROUP_OPTIONS_H
