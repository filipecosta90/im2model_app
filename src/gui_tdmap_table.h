#ifndef TDMap_Table_H
#define TDMap_Table_H

#include <QTableWidget>

#include "gui_tdmap_cell.h"
#include "cv_image_delegate.h"
#include "td_map.hpp"
#include "treeitem.h"
#include "cv_tdmap_cell_image_frame_delegate.h"

class TDMap_Cell;
class TDMap_TableCompare;

class TDMap_Table : public QTableWidget
{
  Q_OBJECT

  public:
    TDMap_Table(QWidget *parent = 0);

    QString currentLocation() const;
    QTableWidgetSelectionRange selectedRange() const;
    void clear();

    void set_tdmap( TDMap* map );
    void set_simulated_images_grid( std::vector< std::vector<cv::Mat> > image_grid );

    /* signal connecting to treeitems with simulation configuration info */
    void connect_thickness_range_number_samples_changes( const TreeItem* item, int item_changes_column );
    void connect_defocus_range_number_samples_changes( const TreeItem* item, int item_changes_column );

    public slots:
      void selectCurrentRow();
    void selectCurrentColumn();
    void update_RowCount_from_thickness_range_number_samples(int signal_item_changed_column );
    void update_ColumnCount_from_defocus_range_number_samples(int signal_item_changed_column );

signals:
    void modified();
    void tdmap_best_match( int x, int y );

    private slots:
      void somethingChanged();

  private:
    enum { MagicNumber = 0x7F51C883 };

    /* class related vars */
    int ColumnCount = 0;
    int RowCount = 0;
    TDMap_Cell*cell(int row, int column) const;

    // Make a member pointer to a new MyDelegate instance
    CvTDMapImageFrameDelegate *image_delegate;
    QWidget *table_parent;

    /* CORE IM2MODEL*/
    TDMap* core_tdmap = nullptr;
    bool _flag_core_tdmap = false;

    /* VISUAL TDMAP vars */
    int _treeitem_thickness_range_number_samples_watch_col;
    int _treeitem_defocus_range_number_samples_watch_col;
    bool _flag_simulated_image_grid = false;
    std::vector< std::vector<cv::Mat> > simulated_image_grid;

};

#endif

