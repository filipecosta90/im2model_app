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

bool TreeItem::enable_highlight_error( int column ){
    _flag_highlight_error[column] = true;
    return true;
}

TreeItem::TreeItem( QVector<QVariant> &data, TreeItem *parent){
  qRegisterMetaType<std::string>("std::string");
  itemData = data;
  parentItem = parent;
  for( int pos = 0; pos < data.size(); pos++ ){
    itemIsCheckableVec.push_back(false);
    itemState.push_back(false);
    fp_checkable_setters.push_back(  boost::function<bool(bool)>() );
    fp_checkable_getters.push_back(  boost::function<bool(void)>() );
    _flag_highlight_error.push_back( false );
  }

  connect(this, SIGNAL( dataChanged( int ) ) , this, SLOT( clean_highlight_status( int ) ) );
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

TreeItem::TreeItem( QVector<QVariant> &data, boost::function<bool(bool)> setter, boost::function<bool(void)> getter, QVector<QVariant> &legend, TreeItem *parent) : TreeItem( data, setter, legend, parent ) {
  fp_data_getter_bool = getter;
  _flag_fp_data_getter_bool = true;
}

TreeItem::TreeItem( QVector<QVariant> &data, boost::function<bool(bool)> setter,  QVector<QVariant> &legend, QVector<bool> editable, TreeItem *parent  ) : TreeItem( data, setter, legend, parent ){
  itemIsEditableVec = editable;
}

TreeItem::TreeItem( QVector<QVariant> &data, boost::function<bool(bool)> setter, boost::function<bool(void)> getter, QVector<QVariant> &legend, QVector<bool> editable, TreeItem *parent  ) : TreeItem( data, setter, getter, legend, parent ){
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

TreeItem::TreeItem( QVector<QVariant> &data, boost::function<bool(int)> setter, boost::function<int(void)> getter, TreeItem *parent  ) : TreeItem( data, setter, parent ){
  fp_data_getter_int = getter;
  _flag_fp_data_getter_int = true;
}

TreeItem::TreeItem( QVector<QVariant> &data, boost::function<bool(int)> setter, QVector<bool> editable, TreeItem *parent  ) : TreeItem( data, setter, parent ){
  itemIsEditableVec = editable;
}

TreeItem::TreeItem( QVector<QVariant> &data, boost::function<bool(int)> setter, boost::function<int(void)> getter, QVector<bool> editable, TreeItem *parent  ) : TreeItem( data, setter, getter, parent ){
  itemIsEditableVec = editable;
}

TreeItem::~TreeItem(){
  qDeleteAll(childItems);
}

bool TreeItem::load_data_from_getter( ){
  bool result = false;
  int column = _fp_data_getter_col_pos;
  if  (column >= 0 && column < itemData.size() ) {
    //call getter on core im2model
    if( ( _fp_data_setter_col_pos == column )
        && (_flag_fp_data_setter_string || _flag_fp_data_setter_bool ||
          _flag_fp_data_setter_int || _flag_fp_data_setter_double ) ){
      QVariant value;
      if( _flag_fp_data_getter_int ){
        value = QVariant::fromValue( fp_data_getter_int() );
      }
      if( _flag_fp_data_getter_bool ){
        value = QVariant::fromValue( fp_data_getter_bool() );
      }
      if( _flag_fp_data_getter_string ){
        value = QVariant::fromValue( fp_data_getter_string() );
      }
      if( _flag_fp_data_getter_double ){
        value = QVariant::fromValue( fp_data_getter_double() );
      }
      emit dataChanged(column);
      emit dataChanged( _variable_name );
      itemData[column] = value;
      result = true;
    }
  }
  return result;
}

bool TreeItem::get_flag_fp_data_getter_bool( ){
  return _flag_fp_data_getter_bool;
}

bool TreeItem::call_fp_data_getter_bool( ){
  std::cout << "calling fp_data_getter_bool" << std::endl;
  return fp_data_getter_bool();
}

bool TreeItem::get_flag_fp_data_setter_bool( ){
  return _flag_fp_data_setter_bool;
}

bool TreeItem::call_fp_data_setter_bool( bool value ){
  std::cout << "calling fp_data_setter_bool" << std::endl;
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
  std::cout << "just testing check getter " << fp_checkable_getters[col]() << std::endl;
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

  try{
    boost::property_tree::ptree pt_data = pt_root.get_child("data_vec");
    int col = 0;
    for ( boost::property_tree::ptree::value_type &pt_data_node : pt_data ){
      std::string value = pt_data_node.second.data();
      setData( col , QVariant( value.c_str() ), Qt::EditRole );
      col++;
    }
  }
  catch(const boost::property_tree::ptree_error &e){
    std::cout << e.what() << std::endl;
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
    std::cout << e.what() << std::endl;
  }
  try{
    boost::property_tree::ptree pt_childs = pt_root.get_child("child_vec");
    int child_num = 0;
    for ( boost::property_tree::ptree::value_type &pt_child_node_v : pt_childs ){
      boost::property_tree::ptree pt_child_node = pt_child_node_v.second;
      TreeItem* _child =  childItems.value( child_num );
      _child->load_data_from_property_tree(pt_child_node);
      child_num++;
    }
  }
  catch(const boost::property_tree::ptree_error &e){
    std::cout << e.what() << std::endl;
  }

  // more work here
  return true;
}

boost::property_tree::ptree* TreeItem::save_data_into_property_tree( ){
  boost::property_tree::ptree* pt = new boost::property_tree::ptree( );
  boost::property_tree::ptree* pt_data = new boost::property_tree::ptree( );
  boost::property_tree::ptree* pt_checked_state = new boost::property_tree::ptree( );
  boost::property_tree::ptree* pt_childs = new boost::property_tree::ptree( );

  for ( QVariant _data: itemData ){
    // Create an unnamed node containing the value
    boost::property_tree::ptree* pt_data_node = new boost::property_tree::ptree( );
    pt_data_node->put("",_data.toString().toStdString());
    // Add this node to the list.
    pt_data->push_back(std::make_pair("data", *pt_data_node));
  }

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
    boost::property_tree::ptree* pt_child_node = _child->save_data_into_property_tree( );
    // Add this node to the list.
    pt_childs->push_back(std::make_pair("child", *pt_child_node));
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
    result = itemIsCheckableVec.at(column );
  }
  return result;
}

bool TreeItem::insertChildren(TreeItem *item){
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

TreeItem *TreeItem::parent(){
  return parentItem;
}

bool TreeItem::set_parent( TreeItem* parent ){
  parentItem = parent;
  return true;
}

bool TreeItem::removeChildren(int position, int count){
  if (position < 0 || position + count > childItems.size()){
    return false;
  }

  for (int row = 0; row < count; ++row){
    delete childItems.takeAt(position);
  }

  return true;
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
      std::cout << "before string "<<  before_string.size() << std::endl;
      before_string.append( value.toString());
      std::cout << "after string "<<  before_string.size() << std::endl;
      itemData[column].setValue( QVariant::fromValue( before_string ));
      emit dataChanged(column);
      emit dataChanged( _variable_name );
      result = true;
    }
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
      result = fp_checkable_setters.at(column)(_boolean_value);
      itemState[column] = _boolean_value;
    }
  }
  if( role == Qt::EditRole ){
    if  (column >= 0 && column < itemData.size() ) {
      if( itemData[column] != value ){
        //call setter on core im2model
        if( ( _fp_data_setter_col_pos == column ) && (_flag_fp_data_setter_string || _flag_fp_data_setter_bool || _flag_fp_data_setter_int || _flag_fp_data_setter_double ) ){
          if( _flag_fp_data_setter_string ){
            std::string t1 = value.toString().toStdString();
            fp_data_setter_string( t1 );
          }
          if( _flag_fp_data_setter_bool ){
            bool t1 = value.toBool();
            fp_data_setter_bool( t1 );
          }
          if( _flag_fp_data_setter_int ){
            int t1 = value.toInt();
            fp_data_setter_int( t1 );
          }
          if( _flag_fp_data_setter_double ){
            double t1 = value.toDouble();
            fp_data_setter_double( t1 );
          }
          itemData[column] = value;
          emit dataChanged( _variable_name );
          emit dataChanged( column );
          result = true;
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
