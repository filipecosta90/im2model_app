
#include <QtGui>
#include "gui_tdmap_cell.h"

TDMap_Cell::TDMap_Cell(){
  setDirty();
}

QTableWidgetItem *TDMap_Cell::clone() const {
  return new TDMap_Cell(*this);
}

void TDMap_Cell::setData(int role, const QVariant &value){
  QTableWidgetItem::setData(role, value);
  if (role == Qt::EditRole)
    setDirty();
}

void TDMap_Cell::setDirty(){
  cacheIsDirty = true;
}

QVariant TDMap_Cell::data(int role) const {
  if (role == Qt::DisplayRole) {
    if (value().isValid()) {
      return value().toString();
    } else {
      return "####";
    }
  } else if (role == Qt::TextAlignmentRole) {
    if (value().type() == QVariant::String) {
      return int(Qt::AlignLeft | Qt::AlignVCenter);
    } else {
      return int(Qt::AlignRight | Qt::AlignVCenter);
    }
  } else {
    return QTableWidgetItem::data(role);
  }
}

const QVariant Invalid;
QVariant TDMap_Cell::value() const {
  return QVariant();
}



