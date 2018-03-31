/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

/*
   treeitem.cpp
   A container for items of data supplied by the simple tree model.
   */

#include "treeitem.h"

#include <QStringList>
#include <QtCore>

#include <iostream>
#include <boost/foreach.hpp>

void TreeItem::set_action_toolBar( CustomToolButton* tool ){
  alignToolButton = tool;
}

CustomToolButton* TreeItem::get_action_toolBar(){
  return alignToolButton;
}

bool TreeItem::has_hightlight_error( int column ){
  return _flag_highlight_error.at(column );
}

bool TreeItem::disable_highlight_error( int column ){
  _flag_highlight_error[column] = false;
  return true;
}

bool TreeItem::enable_highlight_error( std::string varname , int column ){
  bool result = false;
  if ( _variable_name == varname ){
    _flag_highlight_error[column] = true;
    result = true;
  }
  else{
    int number_childs = childCount();
    for( int n = 0; n < number_childs; n++){
      TreeItem* _child =  childItems.value(n);
      const bool _child_result = _child->enable_highlight_error( varname, column );
      if( _child_result ){
        result = true;
      }
    }
  }
  return result;
}

TreeItem::TreeItem( QVector<QVariant> &data, TreeItem *parent) {

  itemData = data;
  parentItem = parent;
  for( int pos = 0; pos < data.size(); pos++ ){
    /* checkable */
    itemIsCheckableVec.push_back(false);
    /* validatable int */
    itemIsValidatableIntVec.push_back(false);
    itemIsValidatableIntTopVec.push_back(false);
    itemIsValidatableIntBottomVec.push_back(false);
    fp_validator_int_range_min.push_back( boost::function<int(void)>() );
    fp_validator_int_range_max.push_back( boost::function<int(void)>() );
    /* validatable double */
    itemIsValidatableDoubleVec.push_back(false);
    itemIsValidatableDoubleTopVec.push_back(false);
    itemIsValidatableDoubleBottomVec.push_back(false);
    fp_validator_double_range_min.push_back( boost::function<double(void)>() );
    fp_validator_double_range_max.push_back( boost::function<double(void)>() );
    /* checkable */
    fp_checkable_setters.push_back(  boost::function<bool(bool)>() );
    fp_checkable_getters.push_back(  boost::function<bool(void)>() );
    /* fp data getters */
    _flag_fp_data_getter_bool_vec.push_back(false);
    fp_data_getter_bool_vec.push_back( boost::function<bool(void)>() );
    _flag_fp_data_getter_double_vec.push_back(false);
    fp_data_getter_double_vec.push_back( boost::function<double(void)>() );
    _flag_fp_data_getter_int_vec.push_back(false);
    fp_data_getter_int_vec.push_back( boost::function<int(void)>() );
    _flag_fp_data_getter_string_vec.push_back(false);
    fp_data_getter_string_vec.push_back( boost::function<std::string(void)>() );

    /* tooltip */
    itemToolTip.push_back(QVariant());
    _flag_itemToolTip.push_back( false );
    itemState.push_back(false);
    _flag_highlight_error.push_back( false );
  }
  connect(this, SIGNAL( dataChanged( int ) ) , this, SLOT( clean_highlight_status( int ) ) );
  connect(this, SIGNAL( dataChanged( int ) ) , this, SLOT( force_layout_change( int ) ) );

}

void TreeItem::force_layout_change( int column ){
  if( parentModel ){
    std::cout << "forcing layout change by var " << _variable_name << std::endl;
    parentModel->force_layout_change();
  }
}

void TreeItem::clean_highlight_status( int column ){
  if( this->has_hightlight_error(column) ){
    disable_highlight_error(column);
  }
}

TreeItem::TreeItem( QVector<QVariant> &data, QVector<bool> editable, TreeItem *parent  ) : TreeItem( data, parent ){
  itemIsEditableVec = editable;
}

TreeItem::TreeItem( QVector<QVariant> &data, boost::function<bool(bool)> setter, QVector<QVariant> &legend, TreeItem *parent) : TreeItem( data, parent ) {
  fp_data_setter_bool = setter;
  _flag_fp_data_setter_bool = true;
  itemLegend = legend;
}

TreeItem::TreeItem( QVector<QVariant> &data, boost::function<bool(bool)> setter,  QVector<QVariant> &legend, QVector<bool> editable, TreeItem *parent  ) : TreeItem( data, setter, legend, parent ){
  itemIsEditableVec = editable;
}

TreeItem::TreeItem( QVector<QVariant> &data, boost::function<bool(std::string)> setter, TreeItem *parent) : TreeItem( data, parent ) {
  fp_data_setter_string = setter;
  _flag_fp_data_setter_string = true;
}

TreeItem::TreeItem( QVector<QVariant> &data, boost::function<bool(std::string)> setter, QVector<bool> editable, TreeItem *parent  ) : TreeItem( data, setter, parent ){
  itemIsEditableVec = editable;
}

TreeItem::TreeItem( QVector<QVariant> &data, boost::function<bool(double)> setter, TreeItem *parent) : TreeItem( data, parent ) {
  fp_data_setter_double = setter;
  _flag_fp_data_setter_double = true;
}

TreeItem::TreeItem( QVector<QVariant> &data, boost::function<bool(double)> setter, QVector<bool> editable, TreeItem *parent  ) : TreeItem( data, setter, parent ){
  itemIsEditableVec = editable;
}


TreeItem::TreeItem( QVector<QVariant> &data, boost::function<bool(int)> setter, TreeItem *parent) : TreeItem( data, parent ) {
  fp_data_setter_int = setter;
  _flag_fp_data_setter_int = true;
}

TreeItem::TreeItem( QVector<QVariant> &data, boost::function<bool(int)> setter, QVector<bool> editable, TreeItem *parent  ) : TreeItem( data, setter, parent ){
  itemIsEditableVec = editable;
}

TreeItem::TreeItem( QVector<QVariant> &data, boost::function<void(QRect)> setter, TreeItem *parent  ) : TreeItem( data, parent ){
  fp_data_setter_rect = setter;
  _flag_fp_data_setter_rect = true;
}

TreeItem::TreeItem( QVector<QVariant> &data, boost::function<void(QRect)> setter, QVector<bool> editable, TreeItem *parent  ) : TreeItem( data, setter, parent ){
  itemIsEditableVec = editable;
}


TreeItem::~TreeItem(){
  qDeleteAll( childItems );
}

void TreeItem::load_data_from_rect( QRect _rect_value ){
  QVariant value = QVariant::fromValue( _rect_value );
  itemData[_fp_data_setter_col_pos] = value;
}

void TreeItem::load_data_from_getter( int column ){
  if  (column >= 0 && column < itemData.size() ) {
    //call getter on core im2model
    QVariant value;
    if( _flag_fp_data_getter_bool_vec[column] == true  ){
      const bool _bool_value = fp_data_getter_bool_vec[column]();
      value = QVariant::fromValue( _bool_value );
      itemData[column] = value;
      emit dataChanged( column );
      emit dataChanged( _variable_name );
    }
    if( _flag_fp_data_getter_rect_vec[column] == true  ){
      const QRect _rect_value = fp_data_getter_rect_vec[column]();
      value = QVariant::fromValue( _rect_value );
      itemData[column] = value;
      emit dataChanged( column );
      emit dataChanged( _variable_name );
    }
    if( _flag_fp_data_getter_double_vec[column] == true  ){
      const double _double_value = fp_data_getter_double_vec[column]();
      value = QVariant::fromValue( _double_value );
      itemData[column] = value;
      emit dataChanged( column );
      emit dataChanged( _variable_name );
    }
    if( _flag_fp_data_getter_int_vec[column] == true  ){
      const int _int_value = fp_data_getter_int_vec[column]();
      value = QVariant::fromValue( _int_value );
      itemData[column] = value;
      emit dataChanged( column );
      emit dataChanged( _variable_name );
    }
    if( _flag_fp_data_getter_string_vec[column] == true  ){
      std::string _string_value = fp_data_getter_string_vec[column]();
      std::cout << "\t\tfp_data_getter_string_vec in col "<< column << " value:\n"<< _string_value <<std::endl;
      //value = QVariant::fromValue( _string_value );
      itemData[column] = QString::fromStdString(_string_value);
      emit dataChanged( column );
      emit dataChanged( _variable_name );
    }
  }
}

// loads data for all cols
void TreeItem::load_data_from_getter_double( ){
  for(int col = 0; col < fp_data_getter_double_vec.size(); col++ ){
    load_data_from_getter( col );
  }
}

// loads data for all cols
void TreeItem::load_data_from_getter_int( ){
  for(int col = 0; col < fp_data_getter_int_vec.size(); col++ ){
    load_data_from_getter( col );
  }
}

// loads data for all cols
void TreeItem::load_data_from_getter_string( ){
  std::cout << "##### load_data_from_getter_string()" << std::endl;
  for(int col = 0; col < fp_data_getter_string_vec.size(); col++ ){
    load_data_from_getter( col );
  }
}

bool TreeItem::get_flag_fp_data_setter_bool( ){
  return _flag_fp_data_setter_bool;
}

bool TreeItem::call_fp_data_setter_bool( bool value ){
  return fp_data_setter_bool( value );
}

void TreeItem::add_toolbar ( QVector<QVariant> actions_description , std::vector<boost::function<bool()>> actions ){
  _toolbar_actions_description = actions_description;
  _toolbar_actions = actions;
  _flag_toolbar = true;
}

QVector<QVariant> TreeItem::get_toolbar_actions_description(){
  return _toolbar_actions_description;
}

bool TreeItem::set_fp_check_setter( int col,  boost::function<bool(bool)> check_setter ){
  itemIsCheckableVec[col] = true;
  fp_checkable_setters[col] = check_setter;
  return true;
}

bool TreeItem::set_fp_check_getter( int col,  boost::function<bool(void)> check_getter ){
  itemIsCheckableVec[col] = true;
  fp_checkable_getters[col] = check_getter;
  return true;
}

bool TreeItem::load_check_status_from_getter( int col ){
  bool _get_result = fp_checkable_getters[col]();
  itemState[col] = _get_result;
  return true;
}

void TreeItem::setStatusOption( int col , ActionStatusType status ){
  _status_column = col;
  _action_status = status;
  _flag_is_action = true;
  setData( col, QVariant(status), Qt::EditRole );
}


std::vector<boost::function<bool()>> TreeItem::get_toolbar_actions(){
  return _toolbar_actions;
}

bool TreeItem::_is_toolbar_defined(){
  return _flag_toolbar;
}

bool TreeItem::load_data_from_property_tree( boost::property_tree::ptree pt_root ){
  bool result = false;
  try{
    const std::string file_variable_name = pt_root.get<std::string>( "varname" );
    //std::cout << "file_variable_name: " << file_variable_name << " | _variable_name: " << _variable_name << std::endl;
    if( file_variable_name == _variable_name ){
      _flag_variable_name = true;
      result = true;
      try{
        boost::property_tree::ptree pt_data = pt_root.get_child("data_vec");
        int col = 0;
        for ( boost::property_tree::ptree::value_type &pt_data_node : pt_data ){

          if( _flag_fp_data_setter_rect && _fp_data_setter_col_pos == col ){
            int x = pt_data_node.second.get<int>("x", 0);
            int y = pt_data_node.second.get<int>("y", 0);
            int width = pt_data_node.second.get<int>("width", 0);
            int height = pt_data_node.second.get<int>("height", 0);
            QRect rect(x,y,width,height);
            setData( col , QVariant( rect ), Qt::EditRole );
          }
          else{
            std::string value = pt_data_node.second.data();
            setData( col , QVariant( value.c_str() ), Qt::EditRole );
          }
          col++;
        }
      }
      catch(const boost::property_tree::ptree_error &e){
        result &= false;
        //std::cout << e.what() << std::endl;
      }
      try{
        boost::property_tree::ptree pt_checked_state = pt_root.get_child("checked_state_vec");
        int col = 0;
        for ( boost::property_tree::ptree::value_type &pt_checked_state_node : pt_checked_state ){
          bool value = pt_checked_state_node.second.get_value<bool>();
          setData( col , QVariant( value ), Qt::CheckStateRole );
          col++;
        }
      }
      catch(const boost::property_tree::ptree_error &e) {
        result &= false;
        //std::cout << e.what() << std::endl;
      }
      try{
        boost::property_tree::ptree pt_childs = pt_root.get_child("child_vec");
        int child_num = 0;
        const int tree_n_childs = childItems.size();
        for ( int child_num = 0; child_num < tree_n_childs ; child_num++ ){
          TreeItem* _child =  childItems.value( child_num );
          const std::string child_varname = _child->get_variable_name();
          boost::property_tree::ptree pt_child_node = pt_childs.get_child(child_varname);
          result &= _child->load_data_from_property_tree(pt_child_node);
          //  std::cout << "result of loading " << child_varname << std::boolalpha << result << std::endl;
        }
        /*
           for ( boost::property_tree::ptree::value_type &pt_child_node_v : pt_childs ){
           boost::property_tree::ptree pt_child_node = pt_child_node_v.second;
           TreeItem* _child =  childItems.value( child_num );
           result &= _child->load_data_from_property_tree(pt_child_node);
           child_num++;
           }
           */
      }
      catch(const boost::property_tree::ptree_error &e){
        std::cout << e.what() << std::endl;
      }
    }
  }
  catch(const boost::property_tree::ptree_error &e){
    result &= false;
  }
  return result;
}

boost::property_tree::ptree* TreeItem::save_data_into_property_tree(){
  boost::property_tree::ptree* pt = new boost::property_tree::ptree();
  boost::property_tree::ptree* pt_data = new boost::property_tree::ptree();
  boost::property_tree::ptree* pt_checked_state = new boost::property_tree::ptree();
  boost::property_tree::ptree* pt_childs = new boost::property_tree::ptree();

  int col = 0;
  for ( QVariant _data: itemData ){
    // Create an unnamed node containing the value
    boost::property_tree::ptree* pt_data_node = new boost::property_tree::ptree();
    if( _flag_fp_data_setter_rect && _fp_data_setter_col_pos == col ){
      QRect rect = _data.toRect();
    pt_data_node->put("x",rect.x() ); //toString().toStdString());
    pt_data_node->put("y",rect.y() ); //toString().toStdString());
    pt_data_node->put("width",rect.width() ); //toString().toStdString());
    pt_data_node->put("height",rect.height() ); //toString().toStdString());
  }
  else{
    pt_data_node->put("",_data.toString().toStdString());
  }
    // Add this node to the list.
  pt_data->push_back(std::make_pair("data", *pt_data_node));
  col++;
}
pt->put("varname", _variable_name );

pt->add_child("data_vec", *pt_data);

for ( bool _checked_state: itemState ){
    // Create an unnamed node containing the value
  boost::property_tree::ptree* pt_checked_state_node = new boost::property_tree::ptree( );
  pt_checked_state_node->put( "", _checked_state );
    // Add this node to the list.
  pt_checked_state->push_back(std::make_pair("checkable", *pt_checked_state_node));
}

pt->add_child("checked_state_vec", *pt_checked_state);

int number_childs = childCount();
for( int n = 0; n < number_childs; n++){
  TreeItem* _child =  childItems.value(n);
  const std::string child_varname = _child->get_variable_name();
  boost::property_tree::ptree* pt_child_node = _child->save_data_into_property_tree( );
    // Add this node to the list.
  pt_childs->push_back(std::make_pair(child_varname, *pt_child_node));
}
pt->add_child("child_vec", *pt_childs );
return pt;
}

TreeItem *TreeItem::child(int number) {
  return childItems.value(number);
}

int TreeItem::childCount() const
{
  return childItems.count();
}

int TreeItem::childNumber() const {
  int child_number = 0;
  if ( parentItem ){
    child_number = parentItem->childItems.indexOf(const_cast<TreeItem*>(this));
  }
  return child_number;
}

int TreeItem::columnCount() const
{
  return itemData.size();
}

QVariant TreeItem::data(int column, int role ) const{
  if ( role == Qt::CheckStateRole ) {
    if( itemIsCheckableVec.at(column) == true ){
      const bool _checked = itemState[column];
      return static_cast< int >( _checked ? Qt::Checked : Qt::Unchecked );
    }
    return QVariant();
  }
  else if( role == Qt::DisplayRole || role ==Qt::EditRole ){
    return itemData.value(column);
  }
  else if( role == Qt::ToolTipRole ){
    std::cout << "data with Qt::ToolTipRole" << std::endl;
    if( _flag_itemToolTip.at(column) == true ) {
      itemToolTip.value(column);
    }else{
      return QVariant();
    }
  }
  else{
    return QVariant();
  }
}


QVariant TreeItem::get_legend(int column) const{
  return itemLegend.value(column);
}

bool TreeItem::isItemEditable( int column ) const{
  bool result = false;
  if (column >= 0 && column < itemIsEditableVec.size()){
    result = itemIsEditableVec.at(column);
  }
  return result;
}

bool TreeItem::isItemCheckable( int column ) const {
  bool result = false;
  if ( column >= 0 && column < itemIsCheckableVec.size() ){
    result = itemIsCheckableVec.at( column );
  }
  return result;
}

bool TreeItem::insertChildren(TreeItem *item){
  if( parentModel != nullptr ){
    item->set_model( parentModel );
    std::cout << "connect " << item->get_variable_name() << " to model " << std::endl;
    connect(item, SIGNAL( dataChanged( int ) ) , parentModel, SLOT( set_model_modified() ) );
  }
  item->set_parent(this);
  childItems.push_back(item);
  return true;
}

bool TreeItem::insertChildren(int position, int count, int columns){
  if (position < 0 || position > childItems.size()){
    return false;
  }
  for (int row = 0; row < count; ++row) {
    QVector< QVariant > data;
    boost::function<bool(std::string)> data_setters;
    TreeItem *item = new TreeItem(data, data_setters, this);
    childItems.insert(position, item);
  }

  return true;
}

std::string TreeItem::get_variable_name(){
  return _variable_name;
}

void TreeItem::set_variable_name( std::string varname ){
  _variable_name = varname;
  _flag_variable_name = true;
}

void TreeItem::set_variable_description( std::string vardescription ){
  _variable_description = vardescription;
}

std::string TreeItem::get_variable_description( ){
  return _variable_description;
}

bool TreeItem::insertColumns(int position, int columns){
  if (position < 0 || position > itemData.size()){
    return false;
  }

  for (int column = 0; column < columns; ++column){
    itemData.insert(position, QVariant() );
  }

  foreach (TreeItem *child, childItems){
    child->insertColumns(position, columns);
  }
  return true;
}
TreeModel* TreeItem::model(){
  return parentModel;
}

TreeItem *TreeItem::parent(){
  return parentItem;
}

bool TreeItem::set_parent( TreeItem* parent ){
  parentItem = parent;
  return true;
}

bool TreeItem::set_model( TreeModel* model ){
  parentModel = model;
  return true;
}

bool TreeItem::removeAllChildren(){
  childItems.clear();
  return true;
}

bool TreeItem::removeChildren(int position, int count){
  bool result = false;
  if (position < 0 || position + count > childItems.size()){
    result = false;
  }
  else{
    for (int row = 0; row < count; ++row){
      delete childItems.takeAt(position);
    }
  }
  return result;
}

bool TreeItem::removeColumns(int position, int columns){
  if (position < 0 || position + columns > itemData.size() ){
    return false;
  }
  for (int column = 0; column < columns; ++column){
    itemData.remove(position);
  }
  foreach (TreeItem *child, childItems)
  child->removeColumns(position, columns);

  return true;
}

int TreeItem::get_dropdown_column(){
  return dropdown_column;
}

int TreeItem::get_slider_int_range_min(){
  return _slider_int_range_min;
}

int TreeItem::get_slider_int_range_max(){
  return _slider_int_range_max;
}

void TreeItem::set_slider_int_range_min( int min ){
  _slider_int_range_min = min;
}

void TreeItem::set_slider_int_range_max( int max ){
  _slider_int_range_max = max;
}

int TreeItem::get_slider_column(){
  return slider_column;
}

bool TreeItem::appendData( int column, const QVariant &value){
  bool result = false;
  if  (column >= 0 && column < itemData.size() ) {
    if( ( _fp_data_data_appender_col_pos == column ) && (_flag_fp_data_appender_string ) ){
      std::string t1 = value.toString().toStdString();
      //  fp_data_appender_string( t1 );
      QString before_string = itemData[column].toString();
      //std::cout << "before string "<<  before_string.size() << std::endl;
      before_string.append( value.toString());
      //std::cout << "after string "<<  before_string.size() << std::endl;
      itemData[column].setValue( QVariant::fromValue( before_string ));
      emit dataChanged(column);
      emit dataChanged( _variable_name );
      result = true;
    }
  }
  return result;
}

bool TreeItem::clearData( int column ){
  bool result = false;
  if  (column >= 0 && column < itemData.size() ) {
    itemData[column].setValue( QVariant() );
    emit dataChanged(column);
    emit dataChanged( _variable_name );
    result = true;
  }
  return result;
}

bool TreeItem::set_fp_data_data_appender_col_pos( int col ){
  _fp_data_data_appender_col_pos = col;
  return true;
}

bool TreeItem::set_flag_fp_data_appender_string( bool set ){
  _flag_fp_data_appender_string = set;
  return true;
}

bool TreeItem::setData(int column, const QVariant &value, int role ){
  bool result = false;
  if( role == Qt::CheckStateRole ){
    if( itemIsCheckableVec.at(column) == true ){
      const bool _boolean_value = value.toBool();
      result = fp_checkable_setters.at(column)( _boolean_value );
      itemState[column] = _boolean_value;
    }
  }
  if( role == Qt::EditRole ){
    if  (column >= 0 && column < itemData.size() ) {
      if( itemData[column] != value ){
        bool setter_result = false;
        //call setter on core im2model
        if( ( _fp_data_setter_col_pos == column )
          && (_flag_fp_data_setter_string
            || _flag_fp_data_setter_bool
            || _flag_fp_data_setter_int
            || _flag_fp_data_setter_double
            || _flag_fp_data_setter_rect  )
          ){
          if( _flag_fp_data_setter_string ){
            std::string t1 = value.toString().toStdString();
            setter_result = fp_data_setter_string( t1 );
          }
          if( _flag_fp_data_setter_rect ){
            QRect t1 = value.toRect();
            fp_data_setter_rect( t1 );
            setter_result = true;
          }
          if( _flag_fp_data_setter_bool ){
            bool t1 = value.toBool();
            setter_result = fp_data_setter_bool( t1 );
          }
          if( _flag_fp_data_setter_int ){
            int t1 = value.toInt();
            setter_result = fp_data_setter_int( t1 );
          }
          if( _flag_fp_data_setter_double ){
            double t1 = value.toDouble();
            setter_result = fp_data_setter_double( t1 );
          }
          if( setter_result ){
            itemData[column] = value;
            emit dataChanged( _variable_name );
            emit dataChanged( column );
            result = true;
          }
          else{
            result = false;
          }
        }
      }
    }
  }
  return result;
}

TreeItem::DelegateType TreeItem::get_item_delegate_type(){
  return _item_delegate_type;
}

void TreeItem::set_item_delegate_type( TreeItem::DelegateType _delegate_type ){
  _item_delegate_type = _delegate_type;
}

void TreeItem::set_dropdown_options(int column , QVector<QVariant>& drop, QVector<QVariant> &drop_enum ){
  dropdown_column = column;
  dropdown_data = drop;
  dropdown_enum = drop_enum;
  _flag_dropdown = true;
}

QVector<QVariant> TreeItem::get_dropdown_data(){
  return dropdown_data;
}

QVector<QVariant> TreeItem::get_dropdown_enum(){
  return  dropdown_enum;
}


/* field validation int */

bool TreeItem::get_flag_validatable_int(int col_pos ){
  bool result = false;
  if  (col_pos >= 0 && col_pos < itemIsValidatableIntVec.size() ) {
    //call getter on core im2model
    result = itemIsValidatableIntVec.at( col_pos );
  }
  return result;
}

bool TreeItem::set_flag_validatable_int(int col_pos , bool value  ){
  bool result = false;
  if  (col_pos >= 0 && col_pos < itemIsValidatableIntVec.size() ) {
    //call getter on core im2model
    itemIsValidatableIntVec[col_pos] = value ;
    result = true;
  }
  return result;
}

bool TreeItem::get_flag_validatable_int_top(int col_pos ){
  bool result = false;
  if  (col_pos >= 0 && col_pos < itemIsValidatableIntTopVec.size() ) {
    //call getter on core im2model
    result = itemIsValidatableIntTopVec.at( col_pos );
  }
  return result;
}

bool TreeItem::get_flag_validatable_int_bottom(int col_pos ){
  bool result = false;
  if  (col_pos >= 0 && col_pos < itemIsValidatableIntBottomVec.size() ) {
    //call getter on core im2model
    result = itemIsValidatableIntBottomVec.at( col_pos );
  }
  return result;
}

bool TreeItem::set_validator_int_top(int col_pos ,  boost::function<int(void)> fp_validator_max ){
  bool result = false;
  if  (col_pos >= 0 && col_pos < fp_validator_int_range_max.size() ) {
    //call getter on core im2model
    fp_validator_int_range_max[col_pos] = fp_validator_max ;
    itemIsValidatableIntTopVec[col_pos] = true;
    result = true;
  }
  return result;
}

bool TreeItem::set_validator_int_bottom(int col_pos ,  boost::function<int(void)> fp_validator_min ){
  bool result = false;
  if  (col_pos >= 0 && col_pos < fp_validator_int_range_min.size() ) {
    //call getter on core im2model
    fp_validator_int_range_min[col_pos] = fp_validator_min ;
    itemIsValidatableIntBottomVec[col_pos] = true;
    result = true;
  }
  return result;
}

int TreeItem::get_validator_value_int_bottom( int col_pos ){
  int value = 0;
  if  (col_pos >= 0 && col_pos < fp_validator_int_range_min.size() ) {
    value = fp_validator_int_range_min[col_pos]();
  }
  return value;
}

bool TreeItem::set_fp_data_getter_bool_vec(int col_pos ,  boost::function<bool(void)> fp ){
  bool result = false;
  if  (col_pos >= 0 && col_pos < fp_data_getter_bool_vec.size() ) {
    //call getter on core im2model
    fp_data_getter_bool_vec[col_pos] = fp ;
    _flag_fp_data_getter_bool_vec[col_pos] = true;
    result = true;
  }
  return result;
}

bool TreeItem::set_fp_data_getter_double_vec(int col_pos ,  boost::function<double(void)> fp ){
  bool result = false;
  if  (col_pos >= 0 && col_pos < fp_data_getter_double_vec.size() ) {
    //call getter on core im2model
    fp_data_getter_double_vec[col_pos] = fp ;
    _flag_fp_data_getter_double_vec[col_pos] = true;
    result = true;
  }
  return result;
}

bool TreeItem::set_fp_data_getter_int_vec(int col_pos ,  boost::function<int(void)> fp ){
  bool result = false;
  if  (col_pos >= 0 && col_pos < fp_data_getter_int_vec.size() ) {
    //call getter on core im2model
    fp_data_getter_int_vec[col_pos] = fp ;
    _flag_fp_data_getter_int_vec[col_pos] = true;
    result = true;
  }
  return result;
}

bool TreeItem::set_fp_data_getter_string_vec(int col_pos ,  boost::function<std::string(void)> fp ){
  bool result = false;
  if  (col_pos >= 0 && col_pos < fp_data_getter_string_vec.size() ) {
    //call getter on core im2model
    fp_data_getter_string_vec[col_pos] = fp ;
    _flag_fp_data_getter_string_vec[col_pos] = true;
    result = true;
  }
  return result;
}

int TreeItem::get_validator_value_int_top( int col_pos  ){
  int value = 1000;
  if  (col_pos >= 0 && col_pos < fp_validator_int_range_max.size() ) {
    value = fp_validator_int_range_max[col_pos]();
  }
  return value;
}

/* field validation double */

bool TreeItem::get_flag_validatable_double(int col_pos ){
  bool result = false;
  if  (col_pos >= 0 && col_pos < itemIsValidatableDoubleVec.size() ) {
    //call getter on core im2model
    result = itemIsValidatableDoubleVec.at( col_pos );
  }
  return result;
}

bool TreeItem::set_flag_validatable_double(int col_pos , bool value  ){
  bool result = false;
  if  (col_pos >= 0 && col_pos < itemIsValidatableDoubleVec.size() ) {
    //call getter on core im2model
    itemIsValidatableDoubleVec[col_pos] = value ;
    result = true;
  }
  return result;
}

bool TreeItem::get_flag_validatable_double_top(int col_pos ){
  bool result = false;
  if  (col_pos >= 0 && col_pos < itemIsValidatableDoubleTopVec.size() ) {
    //call getter on core im2model
    result = itemIsValidatableDoubleTopVec.at( col_pos );
  }
  return result;
}

bool TreeItem::get_flag_validatable_double_bottom(int col_pos ){
  bool result = false;
  if  (col_pos >= 0 && col_pos < itemIsValidatableDoubleBottomVec.size() ) {
    //call getter on core im2model
    result = itemIsValidatableDoubleBottomVec.at( col_pos );
  }
  return result;
}

bool TreeItem::set_validator_double_top(int col_pos ,  boost::function<double(void)> fp_validator_max ){
  bool result = false;
  if  (col_pos >= 0 && col_pos < fp_validator_double_range_max.size() ) {
    //call getter on core im2model
    fp_validator_double_range_max[col_pos] = fp_validator_max ;
    itemIsValidatableDoubleTopVec[col_pos] = true;
    result = true;
  }
  return result;
}

bool TreeItem::set_validator_double_bottom(int col_pos ,  boost::function<double(void)> fp_validator_min ){
  bool result = false;
  if  (col_pos >= 0 && col_pos < fp_validator_double_range_min.size() ) {
    //call getter on core im2model
    fp_validator_double_range_min[col_pos] = fp_validator_min ;
    itemIsValidatableDoubleBottomVec[col_pos] = true;
    result = true;
  }
  return result;
}

double TreeItem::get_validator_value_double_bottom( int col_pos ){
  double value = 0.0f;
  if  (col_pos >= 0 && col_pos < fp_validator_double_range_min.size() ) {
    value = fp_validator_double_range_min[col_pos]();
  }
  return value;
}

double TreeItem::get_validator_value_double_top( int col_pos  ){
  double value = 1000.0f;
  if  (col_pos >= 0 && col_pos < fp_validator_double_range_max.size() ) {
    value = fp_validator_double_range_max[col_pos]();
  }
  return value;
}

void TreeItem::setToolTipText(const QString& text ){
  std::cout << "called setToolTipText with text " << text.toStdString() << std::endl;
}
