#ifndef TDMap_CELL_H
#define TDMap_CELL_H

#include <QTableWidgetItem>

class TDMap_Cell : public QTableWidgetItem
{
  public:
    TDMap_Cell();

    QTableWidgetItem *clone() const;
    void setData(int role, const QVariant &value);
    QVariant data(int role) const;

  private:
    QVariant value() const;

};

#endif
