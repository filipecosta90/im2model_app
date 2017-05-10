/****************************************************************************
 **
 ** Copyright (C) 2005-2006 Trolltech AS. All rights reserved.
 **
 ** This file is part of the documentation of the Qt Toolkit.
 **
 ** This file may be used under the terms of the GNU General Public
 ** License version 2.0 as published by the Free Software Foundation
 ** and appearing in the file LICENSE.GPL included in the packaging of
 ** this file.  Please review the following information to ensure GNU
 ** General Public Licensing requirements will be met:
 ** http://www.trolltech.com/products/qt/opensource.html
 **
 ** If you are unsure which license is appropriate for your use, please
 ** review the following information:
 ** http://www.trolltech.com/products/qt/licensing.html or contact the
 ** sales department at sales@trolltech.com.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ****************************************************************************/

/*
   treeitem.cpp

   A container for items of data supplied by the simple tree model.
   */

#include <QStringList>

#include "treeitem.h"
TreeItem::TreeItem(const QVector<QVariant> &data, TreeItem *parent)
{
  parentItem = parent;
  itemData = data;
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
  return itemData.count();
}

QVariant TreeItem::data(int column) const
{
  return itemData.value(column);
}

bool TreeItem::insertChildren(int position, int count, int columns)
{
  if (position < 0 || position > childItems.size())
    return false;

  for (int row = 0; row < count; ++row) {
    QVector<QVariant> data(columns);
    TreeItem *item = new TreeItem(data, this);
    childItems.insert(position, item);
  }

  return true;
}

bool TreeItem::insertColumns(int position, int columns)
{
  if (position < 0 || position > itemData.size())
    return false;

  for (int column = 0; column < columns; ++column)
    itemData.insert(position, QVariant());

  foreach (TreeItem *child, childItems)
    child->insertColumns(position, columns);

  return true;
}

TreeItem *TreeItem::parent()
{
  return parentItem;
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
  if (position < 0 || position + columns > itemData.size())
    return false;

  for (int column = 0; column < columns; ++column)
    itemData.remove(position);

  foreach (TreeItem *child, childItems)
    child->removeColumns(position, columns);

  return true;
}

bool TreeItem::setData(int column, const QVariant &value)
{
  if (column < 0 || column >= itemData.size())
    return false;

  itemData[column] = value;
  return true;
}
