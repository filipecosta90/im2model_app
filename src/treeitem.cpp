/*
   treeitem.cpp
   A container for items of data supplied by the simple tree model.
   */

#include "treeitem.h"

#include <QStringList>
#include <QtCore>

#include <iostream>

TreeItem::TreeItem( QVector<QVariant> &data, boost::function<bool(std::string)> setter, QVector<bool> editable, TreeItem *parent  )
{
  itemData = data;
  fp_data_setter = setter;
  parentItem = parent;
  is_checkable = false;
  itemIsEditableVec = editable;
}

TreeItem::TreeItem( QVector<QVariant> &data, boost::function<bool(std::string)> setter, TreeItem *parent, bool checkable, boost::function<bool(bool)> check_setter)
{
  itemData = data;
  fp_data_setter = setter;
  parentItem = parent;
  is_checkable = checkable;
  fp_check_setter =  check_setter;
}

TreeItem::TreeItem( QVector<QVariant> &data, boost::function<bool(std::string)> setter, TreeItem *parent)
{
  itemData = data;
  fp_data_setter = setter;
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
  if (parentItem)
    return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));

  return 0;
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
  if (position < 0 || position > childItems.size())
    return false;

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
  if (position < 0 || position + count > childItems.size())
    return false;

  for (int row = 0; row < count; ++row)
    delete childItems.takeAt(position);

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

bool TreeItem::setData(int column, const QVariant &value)
{
  bool result = false;
  if  (column >= 0 && column < itemData.size() ) {
    //call setter on core im2model
    std::string t1 = value.toString().toStdString();
    fp_data_setter( t1 );
    if(itemData[column] != value ){
        emit dataChanged(column);
        itemData[column] = value;
    }
    result = true;
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

