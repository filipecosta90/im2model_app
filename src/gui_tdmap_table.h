#ifndef TDMap_Table_H
#define TDMap_Table_H

#include <QTableWidget>

#include "gui_tdmap_cell.h"
#include "cv_image_delegate.h"
#include "td_map.hpp"
#include "treeitem.h"

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

    void set_tdmap( TDMap* map );
    void connect_thickness_changes( const TreeItem* thickness_object, int column );

    public slots:
    void selectCurrentRow();
    void selectCurrentColumn();
    void recalculate();
    void setAutoRecalculate(bool recalc);
    void update_map_size_defocus(int column );

signals:
    void modified();

    private slots:
      void somethingChanged();

  private:
    enum { MagicNumber = 0x7F51C883 };
int ColumnCount = 26;
    int RowCount = 999;
    TDMap_Cell*cell(int row, int column) const;
    QString text(int row, int column) const;
    QString formula(int row, int column) const;
    void setFormula(int row, int column, const QString &formula);
    bool autoRecalc;
    // Make a member pointer to a new MyDelegate instance
    CvImageDelegate *image_delegate;
    QWidget *table_parent;

    /* CORE IM2MODEL*/
    TDMap* core_tdmap;
     int _item_model_thickness_range_col;
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

