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
    void setDirty();

  private:
    QVariant value() const;

    mutable QVariant cachedValue;
    mutable bool cacheIsDirty;
};

#endif
