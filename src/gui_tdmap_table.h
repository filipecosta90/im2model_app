#ifndef TDMap_Table_H
#define TDMap_Table_H

#include <QTableWidget>

#include "gui_tdmap_cell.h"

class TDMap_Cell;
class TDMap_TableCompare;

class TDMap_Table : public QTableWidget
{
  Q_OBJECT

  public:
    TDMap_Table(QWidget *parent = 0);

    bool autoRecalculate() const { return autoRecalc; }
    QString currentLocation() const;
    QString currentFormula() const;
    QTableWidgetSelectionRange selectedRange() const;
    void clear();

    public slots:
      void selectCurrentRow();
    void selectCurrentColumn();
    void recalculate();
    void setAutoRecalculate(bool recalc);

signals:
    void modified();

    private slots:
      void somethingChanged();

  private:
    enum { MagicNumber = 0x7F51C883, RowCount = 999, ColumnCount = 26 };

    TDMap_Cell*cell(int row, int column) const;
    QString text(int row, int column) const;
    QString formula(int row, int column) const;
    void setFormula(int row, int column, const QString &formula);

    bool autoRecalc;
};

class TDMap_TableCompare
{
  public:
    bool operator()(const QStringList &row1,
        const QStringList &row2) const;

    enum { KeyCount = 3 };
    int keys[KeyCount];
    bool ascending[KeyCount];
};

#endif

