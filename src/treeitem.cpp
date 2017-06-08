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

TreeItem::TreeItem( QVector<QVariant> &data, TreeItem *parent){
  itemData = data;
  parentItem = parent;
}

TreeItem::TreeItem( QVector<QVariant> &data, boost::function<bool(std::string)> setter, TreeItem *parent) : TreeItem( data, parent ) {
  fp_data_setter_string = setter;
  _flag_fp_data_setter_string = true;
}

TreeItem::TreeItem( QVector<QVariant> &data, boost::function<bool(double)> setter, TreeItem *parent) : TreeItem( data, parent ) {
  fp_data_setter_double = setter;
  _flag_fp_data_setter_double = true;
}

TreeItem::TreeItem( QVector<QVariant> &data, boost::function<bool(bool)> setter, QVector<QVariant> &legend, TreeItem *parent) : TreeItem( data, parent ) {
  fp_data_setter_bool = setter;
  _flag_fp_data_setter_bool = true;
  itemLegend = legend;
}

TreeItem::TreeItem( QVector<QVariant> &data, boost::function<bool(int)> setter, TreeItem *parent) : TreeItem( data, parent ) {
  fp_data_setter_int = setter;
  _flag_fp_data_setter_int = true;
}

TreeItem::TreeItem( QVector<QVariant> &data, boost::function<bool(std::string)> setter, QVector<bool> editable, TreeItem *parent  ) : TreeItem( data, setter, parent ){
  itemIsEditableVec = editable;
}

TreeItem::TreeItem( QVector<QVariant> &data, boost::function<bool(bool)> setter,  QVector<QVariant> &legend, QVector<bool> editable, TreeItem *parent  ) : TreeItem( data, setter, legend, parent ){
  itemIsEditableVec = editable;
}

TreeItem::TreeItem( QVector<QVariant> &data, boost::function<bool(double)> setter, QVector<bool> editable, TreeItem *parent  ) : TreeItem( data, setter, parent ){
  itemIsEditableVec = editable;
}

TreeItem::TreeItem( QVector<QVariant> &data, boost::function<bool(int)> setter, QVector<bool> editable, TreeItem *parent  ) : TreeItem( data, setter, parent ){
  itemIsEditableVec = editable;
}

TreeItem::TreeItem( QVector<QVariant> &data, boost::function<bool(std::string)> setter, bool checkable, boost::function<bool(bool)> check_setter, TreeItem *parent) : TreeItem( data, setter, parent ){
  is_checkable = checkable;
  fp_check_setter =  check_setter;
}

TreeItem::TreeItem( QVector<QVariant> &data, boost::function<bool(double)> setter, bool checkable, boost::function<bool(bool)> check_setter, TreeItem *parent) : TreeItem( data, setter, parent ){
  is_checkable = checkable;
  fp_check_setter =  check_setter;
}

TreeItem::TreeItem( QVector<QVariant> &data, boost::function<bool(int)> setter, bool checkable, boost::function<bool(bool)> check_setter, TreeItem *parent) : TreeItem( data, setter, parent ){
  is_checkable = checkable;
  fp_check_setter =  check_setter;
}

TreeItem::~TreeItem(){
  qDeleteAll(childItems);
}


void TreeItem::add_toolbar ( QVector<QVariant> actions_description , std::vector<boost::function<bool()>> actions ){
  _toolbar_actions_description = actions_description;
  _toolbar_actions = actions;
  _flag_toolbar = true;
}

QVector<QVariant> TreeItem::get_toolbar_actions_description(){
  return _toolbar_actions_description;
}

bool TreeItem::set_fp_checker( int col,  boost::function<bool(bool)> check_setter ){
    _fp_check_setter_col_pos = col;
    _flag_fp_check_setter = true;
fp_check_setter = check_setter;
}

std::vector<boost::function<bool()>> TreeItem::get_toolbar_actions(){
  return _toolbar_actions;
}

bool TreeItem::_is_toolbar_defined(){
  return _flag_toolbar;
}

bool TreeItem::load_data_from_property_tree( boost::property_tree::ptree pt_root ){

  boost::property_tree::ptree pt_data = pt_root.get_child("data_vec");
  boost::property_tree::ptree pt_editable = pt_root.get_child("editable_vec");
  boost::property_tree::ptree pt_childs = pt_root.get_child("child_vec");

  int col = 0;

  for ( boost::property_tree::ptree::value_type &pt_data_node : pt_data ){
    std::string value = pt_data_node.second.data();
    std::cout << "LOADED value " << value << std::endl;
    setData( col , QVariant( value.c_str() ));
    col++;
  }

  col = 0;
  for ( boost::property_tree::ptree::value_type &pt_editable_node : pt_editable ){
    bool value = pt_editable_node.second.get_value<bool>();
    std::cout << "LOADED editable " << value << std::endl;
    itemIsEditableVec[col]=value;
    col++;
  }

  int child_num = 0;
  for ( boost::property_tree::ptree::value_type &pt_child_node_v : pt_childs ){
    boost::property_tree::ptree pt_child_node = pt_child_node_v.second;
    TreeItem* _child =  childItems.value( child_num );
    _child->load_data_from_property_tree(pt_child_node);
    child_num++;
  }

  // more work here
  return true;
}

boost::property_tree::ptree* TreeItem::save_data_into_property_tree( ){
  boost::property_tree::ptree* pt = new boost::property_tree::ptree( );
  boost::property_tree::ptree* pt_data = new boost::property_tree::ptree( );
  boost::property_tree::ptree* pt_editable = new boost::property_tree::ptree( );
  boost::property_tree::ptree* pt_childs = new boost::property_tree::ptree( );

  for ( QVariant _data: itemData )
  {
    std::cout << "itemData " << _data.toString().toStdString() << std::endl;
    // Create an unnamed node containing the value
    boost::property_tree::ptree* pt_data_node = new boost::property_tree::ptree( );
    pt_data_node->put("",_data.toString().toStdString());
    // Add this node to the list.
    pt_data->push_back(std::make_pair("data", *pt_data_node));
  }

  pt->add_child("data_vec", *pt_data);

  for ( bool _editable: itemIsEditableVec )
  {
    std::cout << "editable " << _editable << std::endl;
    // Create an unnamed node containing the value
    boost::property_tree::ptree* pt_editable_node = new boost::property_tree::ptree( );
    pt_editable_node->put( "", _editable );
    // Add this node to the list.
    pt_editable->push_back(std::make_pair("editable", *pt_editable_node));
  }

  pt->add_child("editable_vec", *pt_editable);

  int number_childs = childCount();
  for( int n = 0; n < number_childs; n++){
    std::cout << "child " << n << std::endl;
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

QVariant TreeItem::data(int column) const{
  return itemData.value(column);
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

int TreeItem::get_checkbox_column(){
    return _fp_check_setter_col_pos;
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


bool TreeItem::appendData( int column, const QVariant &value){
    bool result = false;
    if  (column >= 0 && column < itemData.size() ) {
        if( ( _fp_data_data_appender_col_pos == column ) && (_flag_fp_data_appender_string ) ){
          std::string t1 = value.toString().toStdString();
        //  fp_data_appender_string( t1 );
          QString before_string = itemData[column].toString();
          before_string.append( value.toString());
              itemData[column].setValue(QVariant::fromValue(before_string));
              emit dataChanged(column);
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

bool TreeItem::setData(int column, const QVariant &value){
  bool result = false;
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
        emit dataChanged(column);
        itemData[column] = value;
        result = true;
      }
    }
  }
  return result;
}

bool TreeItem::isChecked() const {
  return checked;
}

void TreeItem::setChecked( bool set ){
  checked = set;
  //call [] check setter on core im2model
  fp_check_setter(set);
}

bool TreeItem::isCheckable() const{
  return is_checkable;
}

void TreeItem::setCheckable( bool set ){
  is_checkable = set;
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


