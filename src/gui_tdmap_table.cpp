#include <QtGui>

#include "gui_tdmap_cell.h"
#include "gui_tdmap_table.h"
#include "cv_tdmap_cell_image_frame_delegate.h"
#include "cv_tdmap_cell_image_frame.h"
#include "cv_image_cell_widget.h"
#include "cv_image_frame.h"


#include "td_map.hpp"

TDMap_Table::TDMap_Table(QWidget *parent) : QTableWidget(parent) {

  this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

  table_parent = parent;
  //image_delegate = new CvTDMapImageFrameDelegate(this);

  //setItemPrototype(new TDMap_Cell);
  setSelectionMode(ContiguousSelection);
  //setItemDelegate(image_delegate);

  connect(this, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(somethingChanged()));
  clear();
}

void TDMap_Table::set_tdmap( TDMap* map ){
  core_tdmap = map;
  _flag_core_tdmap = true;
}

void TDMap_Table::connect_thickness_range_number_samples_changes( const TreeItem* item, int item_changes_column ){
  connect( item, SIGNAL(dataChanged(int )), this, SLOT( update_RowCount_from_thickness_range_number_samples(int) ) );
  _treeitem_thickness_range_number_samples_watch_col = item_changes_column;
}

void TDMap_Table::connect_defocus_range_number_samples_changes( const TreeItem* item, int item_changes_column ){
  connect( item, SIGNAL(dataChanged( int )), this, SLOT( update_ColumnCount_from_defocus_range_number_samples(int) ) );
  _treeitem_defocus_range_number_samples_watch_col = item_changes_column;
}

void TDMap_Table::update_RowCount_from_thickness_range_number_samples( int signal_item_changed_column ){
  if ( signal_item_changed_column == _treeitem_thickness_range_number_samples_watch_col ){
    int new_RowCount = core_tdmap->get_thickness_range_number_samples();
    RowCount = new_RowCount;
    std::cout << "new_RowCount " << new_RowCount << std::endl;
    clear();
  }
}

void TDMap_Table::update_ColumnCount_from_defocus_range_number_samples( int signal_item_changed_column ){
  if ( signal_item_changed_column == _treeitem_defocus_range_number_samples_watch_col ){
    int new_ColumnCount = core_tdmap->get_defocus_range_number_samples();
    ColumnCount = new_ColumnCount;
    std::cout << "new_ColumnCount " << new_ColumnCount << std::endl;
    clear();
  }
}

void TDMap_Table::set_simulated_images_grid( std::vector< std::vector<cv::Mat> > image_grid ){
  simulated_image_grid = image_grid;
  _flag_simulated_image_grid = true;
  clear();
}

void TDMap_Table::clear()
{
  setRowCount(0);
  setColumnCount(0);
  setRowCount(RowCount);
  setColumnCount(ColumnCount);
  double _defocus_lower = 0.0f;
  double _defocus_period = 0.0f;
  int _thickness_lower_slice = 0;
  int _thickness_period_slice = 0;
  if( _flag_core_tdmap ){
    if( core_tdmap->_is_defocus_range_lower_bound_defined() ){
      _defocus_lower = core_tdmap->get_defocus_range_lower_bound();
    }
    if(  core_tdmap->_is_defocus_period_defined() ){
      _defocus_period = core_tdmap->get_defocus_range_period();
    }

    if( core_tdmap->_is_thickness_range_lower_bound_slice_defined() ){
      _thickness_lower_slice = core_tdmap->get_thickness_range_lower_bound_slice();
    }
    if( core_tdmap->_is_thickness_period_slice_defined() ){
      _thickness_period_slice = core_tdmap->get_thickness_range_period_slice();
    }
  }

  for (int i = 0; i < ColumnCount; ++i) {
    QTableWidgetItem *item = new QTableWidgetItem;
    double _at_defocus = _defocus_lower + ( _defocus_period * i );
    item->setText( QString::number( _at_defocus ) );
    setHorizontalHeaderItem(i, item);
  }
  std::vector <double> _local_accum_nm_slice_vec;
  bool use_accum_nm = false;
  if ( core_tdmap ){
    if( core_tdmap->_is_celslc_accum_nm_slice_vec_defined() ){
      use_accum_nm = true;
      _local_accum_nm_slice_vec = core_tdmap->get_celslc_accum_nm_slice_vec();
    }
  }
  for (int i = 0; i < RowCount; ++i) {
    QTableWidgetItem *item = new QTableWidgetItem;
    const int at_slice = round( _thickness_lower_slice + _thickness_period_slice * ( i )  );
    QString legend;
    if( use_accum_nm ){
      const double slice_thickness_nm = _local_accum_nm_slice_vec.at(at_slice-1);
      //const double slice_thickness_nm = _accum_nm_slice_vec.at(at_slice-1);
      legend = QString::number( slice_thickness_nm ) + QString( "nm  #" ) + QString::number( at_slice );
    }
    else {
      legend = QString::number( at_slice );
    }
    item->setText( legend );
    setVerticalHeaderItem(i, item);
  }


  if( _flag_simulated_image_grid ){
    cv::Point2i best_match_pos = core_tdmap->get_simgrid_best_match_position();

    for (int row = 0; row < RowCount; ++row) {
      std::vector<cv::Mat> simulated_image_row = simulated_image_grid.at(row);
      for (int col = 0; col < ColumnCount; ++col) {
        CvImageCellWidget *cell_widget  = new CvImageCellWidget();
        cv::Mat full_image = simulated_image_row.at(col);
        cell_widget->setImage( full_image.clone() );
        //cell_widget->setImage( full_image.clone() );
        this->setCellWidget(row,col, cell_widget);
        // cell_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding );
        //  cell_widget->setMinimumSize( );
        //horizontalHeader()->setResizeMode(QHeaderView::Stretch);
        //verticalHeader()->setResizeMode(QHeaderView::Stretch);
      }
    }

    this->resizeColumnsToContents();
    this->resizeRowsToContents();

    std::cout << "setting best match position to: " << best_match_pos << std::endl;

    setCurrentCell(best_match_pos.x, best_match_pos.y);
    std::cout << "finished creating Image frames " << std::endl;
  }
  else{
    setCurrentCell(0, 0);
  }
}

/*
 * The cell() private function returns the TDMap_Cellobject for a given row and column. 
 * It is almost the same as QTableWidget::item(), 
 * except that it returns a TDMap_Cellpointer instead of a QTableWidgetItem pointer.
 * */
TDMap_Cell* TDMap_Table::cell(int row, int column) const
{
  return static_cast<TDMap_Cell*> (item(row, column));
}


/*
 * The currentLocation() function returns the current cell's location in the 
 * usual TDMap_Table format of column letter followed by row number. 
 * MainWindow::updateStatusBar() uses it to show the location in the status bar.
 * */
QString TDMap_Table::currentLocation() const
{
  return QChar('A' + currentColumn())
    + QString::number(currentRow() + 1);
}

/*
 * The somethingChanged() private slot recalculates the whole TDMap_Table if 
 * "auto-recalculate" is enabled. It also emits the modified() signal.
 */
void TDMap_Table::somethingChanged()
{
  emit modified();
}

/*
 * The selectCurrentRow() and selectCurrentColumn() functions correspond to the Edit|Select|Row and Edit|Select|Column menu options. The implementations rely on QTableWidget's selectRow() and selectColumn() functions. We do not need to implement the functionality behind Edit|Select|All, since that is provided by QTableWidget's inherited function QAbstractItemView::selectAll().
 * */
void TDMap_Table::selectCurrentRow()
{
  selectRow(currentRow());
}

void TDMap_Table::selectCurrentColumn()
{
  selectColumn(currentColumn());
}


