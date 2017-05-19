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

    /* signal connecting to treeitems with simulation configuration info */
    void connect_thickness_range_number_samples_changes( const TreeItem* item, int item_changes_column );
    void connect_defocus_range_number_samples_changes( const TreeItem* item, int item_changes_column );

    public slots:
      void selectCurrentRow();
    void selectCurrentColumn();
    void recalculate();
    void setAutoRecalculate(bool recalc);
    void update_RowCount_from_thickness_range_number_samples(int signal_item_changed_column );
    void update_ColumnCount_from_defocus_range_number_samples(int signal_item_changed_column );

signals:
    void modified();

    private slots:
      void somethingChanged();

  private:
    enum { MagicNumber = 0x7F51C883 };
    int ColumnCount = 0;
    int RowCount = 0;
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

    /* VISUAL TDMAP vars */
    int _treeitem_thickness_range_number_samples_watch_col;
    int _treeitem_defocus_range_number_samples_watch_col;

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

