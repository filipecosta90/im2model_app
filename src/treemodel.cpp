#include <QtGui>
#include "treeitem.h"
#include "treemodel.h"
#include <iostream>

  TreeModel::TreeModel(const QStringList &headers, const QStringList &data, QObject *parent)
: QAbstractItemModel(parent)
{
  QVector<QVariant> rootData;
  foreach (QString header, headers){
    rootData << header;
  }

  //rootItem = new TreeItem(rootData);
  // setupModelData(data, rootItem);
}

  TreeModel::TreeModel(TreeItem *root,  QObject *parent)
: QAbstractItemModel(parent)
{
  rootItem = root;
}

TreeModel::~TreeModel(){
  delete rootItem;
}

boost::property_tree::ptree* TreeModel::save_data_into_property_tree( ){
    // Create an empty property tree object
    boost::property_tree::ptree* pt_root =  rootItem->save_data_into_property_tree( );
    return pt_root;
}


int TreeModel::columnCount(const QModelIndex & /* parent */) const{
  return rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const{
  if (!index.isValid()){
    return QVariant();
  }
  TreeItem *item = getItem(index);
  if ( role == Qt::CheckStateRole && index.column() == 0 && item->isCheckable() ){
    return static_cast< int >( item->isChecked() ? Qt::Checked : Qt::Unchecked );
  }

  if (role != Qt::DisplayRole && role != Qt::EditRole){
    return QVariant();
  }
  return item->data(index.column());
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
  if (!index.isValid())
    return 0;

  TreeItem *item = getItem(index);
  Qt::ItemFlags flags =  QAbstractItemModel::flags(index);
  if( item->isItemEditable( index.column() ) ){
    flags |= Qt::ItemIsEditable;
  }
  if( index.column() == 0 ){
    flags |= Qt::ItemIsUserCheckable;
  }
  return flags;
}

TreeItem *TreeModel::getItem(const QModelIndex &index) const
{
  if (index.isValid()) {
    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    if (item)
      return item;
  }
  return rootItem;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
    int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    return rootItem->data(section);

  return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
  if (parent.isValid() && parent.column() != 0)
    return QModelIndex();

  TreeItem *parentItem = getItem(parent);

  TreeItem *childItem = parentItem->child(row);
  if (childItem)
    return createIndex(row, column, childItem);
  else
    return QModelIndex();
}

bool TreeModel::insertColumns(int position, int columns, const QModelIndex &parent)
{
  bool success;

  beginInsertColumns(parent, position, position + columns - 1);
  success = rootItem->insertColumns(position, columns);
  endInsertColumns();

  return success;
}

bool TreeModel::insertRows(int position, int rows, const QModelIndex &parent)
{
  TreeItem *parentItem = getItem(parent);
  bool success;

  beginInsertRows(parent, position, position + rows - 1);
  success = parentItem->insertChildren(position, rows, rootItem->columnCount());
  endInsertRows();

  return success;
}

QModelIndex TreeModel::parent(const QModelIndex &index) const{
  if (!index.isValid())
    return QModelIndex();

  TreeItem *childItem = getItem(index);
  TreeItem *parentItem = childItem->parent();

  if (parentItem == rootItem)
    return QModelIndex();

  return createIndex(parentItem->childNumber(), 0, parentItem);
}

bool TreeModel::removeColumns(int position, int columns, const QModelIndex &parent){
  bool success;

  beginRemoveColumns(parent, position, position + columns - 1);
  success = rootItem->removeColumns(position, columns);
  endRemoveColumns();

  if (rootItem->columnCount() == 0)
    removeRows(0, rowCount());

  return success;
}

bool TreeModel::removeRows(int position, int rows, const QModelIndex &parent)
{
  TreeItem *parentItem = getItem(parent);
  bool success = true;

  beginRemoveRows(parent, position, position + rows - 1);
  success = parentItem->removeChildren(position, rows);
  endRemoveRows();

  return success;
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
  TreeItem *parentItem = getItem(parent);
  return parentItem->childCount();
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role){

  if ( (role != Qt::EditRole) && ( role != Qt::CheckStateRole) ){
    return false;
  }
  TreeItem *item = getItem(index);
  bool result = false;
  if(role == Qt::CheckStateRole){
    if(item->isChecked()){
      item->setChecked(false);
    }
    else{
      item->setChecked(true);
    }
    result = true;
  }
  if(role == Qt::EditRole ){
    result = item->setData(index.column(), value);
  }
  if (result){
    emit dataChanged(index, index);
  }
  return result;
}

bool TreeModel::setHeaderData(int section, Qt::Orientation orientation,
    const QVariant &value, int role)
{
  if (role != Qt::EditRole || orientation != Qt::Horizontal)
    return false;

  bool result = rootItem->setData(section, value);
  if (result){
    emit headerDataChanged(orientation, section, section);
  }
  return result;
}

QStringList TreeModel::extractStringsFromModel()
{
    return rootItem->extractStringsFromItem();
}
