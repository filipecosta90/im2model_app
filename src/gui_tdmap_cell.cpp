
#include <QtGui>
#include "gui_tdmap_cell.h"

TDMap_Cell::TDMap_Cell(){
}

QTableWidgetItem *TDMap_Cell::clone() const {
  return new TDMap_Cell(*this);
}

void TDMap_Cell::setData(int role, const QVariant &value){
  QTableWidgetItem::setData(role, value);
}

QVariant TDMap_Cell::data(int role) const {
  if (role == Qt::DisplayRole) {
    if (value().isValid()) {
      return value().toString();
    }
  }
  else {
    return QTableWidgetItem::data(role);
  }
}

QVariant TDMap_Cell::value() const {
  return QVariant();
}
