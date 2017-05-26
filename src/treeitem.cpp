/*
   treeitem.cpp
   A container for items of data supplied by the simple tree model.
   */

#include "treeitem.h"

#include <QStringList>
#include <QtCore>

#include <iostream>
#include <boost/foreach.hpp>


TreeItem::TreeItem( QVector<QVariant> &data, boost::function<bool(std::string)> setter, QVector<bool> editable, TreeItem *parent  )
{
  itemData = data;
  fp_data_setter = setter;
  _flag_fp_data_setter = true;
  parentItem = parent;
  is_checkable = false;
  itemIsEditableVec = editable;
}

TreeItem::TreeItem( QVector<QVariant> &data, boost::function<bool(std::string)> setter, TreeItem *parent, bool checkable, boost::function<bool(bool)> check_setter)
{
  itemData = data;
  fp_data_setter = setter;
  _flag_fp_data_setter = true;
  parentItem = parent;
  is_checkable = checkable;
  fp_check_setter =  check_setter;
}

TreeItem::TreeItem( QVector<QVariant> &data, boost::function<bool(std::string)> setter, TreeItem *parent)
{
  itemData = data;
  fp_data_setter = setter;
  _flag_fp_data_setter = true;
  parentItem = parent;
  is_checkable = false;
}

TreeItem::TreeItem( QVector<QVariant> &data, TreeItem *parent)
{
  itemData = data;
  parentItem = parent;
  is_checkable = false;
}

TreeItem::~TreeItem()
{
  qDeleteAll(childItems);
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

TreeItem *TreeItem::child(int number)
{
  return childItems.value(number);
}

int TreeItem::childCount() const
{
  return childItems.count();
}

int TreeItem::childNumber() const
{
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

QVariant TreeItem::data(int column) const
{
  return itemData.value(column);
}

bool TreeItem::isItemEditable( int column ) const{
  bool result = false;
  if (column >= 0 && column < itemIsEditableVec.size()){
    result = itemIsEditableVec.at(column);
  }
  return result;
}

bool TreeItem::insertChildren(TreeItem *item){
  item->set_parent(this);
  childItems.push_back(item);
  return true;
}

bool TreeItem::insertChildren(int position, int count, int columns)
{
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

bool TreeItem::insertColumns(int position, int columns)
{
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

TreeItem *TreeItem::parent()
{
  return parentItem;
}

bool TreeItem::set_parent( TreeItem* parent )
{
  parentItem = parent;
  return true;
}

bool TreeItem::removeChildren(int position, int count)
{
  if (position < 0 || position + count > childItems.size()){
    return false;
  }

  for (int row = 0; row < count; ++row){
    delete childItems.takeAt(position);
  }

  return true;
}

bool TreeItem::removeColumns(int position, int columns)
{
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

bool TreeItem::setData(int column, const QVariant &value){
  bool result = false;
  if  (column >= 0 && column < itemData.size() ) {
    if( itemData[column] != value ){
      //call setter on core im2model
      std::string t1 = value.toString().toStdString();
      std::cout << " _flag_fp_data_setter " << _flag_fp_data_setter << std::endl;
      if( _flag_fp_data_setter && (_fp_data_setter_col_pos == column) ){
          std::cout << "calling setter with value " << t1 << std::endl;
        fp_data_setter( t1 );
        emit dataChanged(column);
        itemData[column] = value;
        std::cout << "setted new value in data " << t1 << std::endl;
        result = true;
      }
    }
  }
  return result;
}

bool TreeItem::isChecked() const
{
  return checked;
}

void TreeItem::setChecked( bool set )
{
  checked = set;
  //call [] check setter on core im2model
  fp_check_setter(set);
}

bool TreeItem::isCheckable() const
{
  return is_checkable;
}

void TreeItem::setCheckable( bool set )
{
  is_checkable = set;
}

QStringList TreeItem::extractStringsFromItem()
{
  QStringList retval;

  for(int column = 0 ; column < itemData.size(); column++ ){
    retval << itemData.value(column).toString();
    std::cout << itemData.value(column).toString().toStdString() << std::endl;
  }
  for(int row = 0 ; row < childItems.size(); row++ ){
    TreeItem* _child_item = childItems.at(row);
    retval << _child_item->extractStringsFromItem( );
  }
  return retval;
}

DelegateType TreeItem::get_item_delegate_type(){
    return _item_delegate_type;
}

void TreeItem::set_item_delegate_type( DelegateType _delegate_type ){
    _item_delegate_type = _delegate_type;
}
