#include "gui_tdmap_table.h"

#include "gui_tdmap_cell.h"
#include "cv_tdmap_cell_image_frame_delegate.h"
#include "cv_tdmap_cell_image_frame.h"
#include "cv_image_cell_widget.h"
#include "cv_image_frame.h"

#include "td_map.hpp"

TDMap_Table::TDMap_Table(QWidget *parent) : QTableWidget(parent) {

  this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

  table_parent = parent;
  this->setStyleSheet("QTableWidget::item { padding: 3px }");

  image_delegate = new CvTDMapImageFrameDelegate( 3 , this );

  // setItemPrototype(new CvImageCellWidget);
  setSelectionMode(ContiguousSelection);
  this->setItemDelegate(image_delegate);

  connect(this, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(somethingChanged()));
  clear();
}

void TDMap_Table::resizeEvent(QResizeEvent* ev) {
  update_column_size();
  update_row_size();
  // Call our base resizeEvent to handle the resizing itself
  // we don't bother with here
  QTableView::resizeEvent(ev);
  clear();
}

void TDMap_Table::set_tdmap( TDMap* map ){
  core_tdmap = map;
  _flag_core_tdmap = true;
}

void TDMap_Table::connect_item_changes_to_invalidate_grid( const TreeItem* item, int item_changes_column ){
  connect( item, SIGNAL(dataChanged( int )), this, SLOT( invalidate_grid(int) ) );
  _treeitem_changes_to_invalidate_grid_watch_col = item_changes_column;
}

void TDMap_Table::invalidate_grid( int signal_item_changed_column ){
  if ( signal_item_changed_column == _treeitem_changes_to_invalidate_grid_watch_col ){
    if( _flag_simulated_image_grid ){
      std::cout << "cleaned grid due to data change in connected TreeItems" << std::endl;
      _flag_simulated_image_grid = false;
      for( int row = 0; row < simulated_image_grid.size() ; row++ ){
        std::vector<cv::Mat> row_image_grid = simulated_image_grid.at(row);
        row_image_grid.clear();
      }
      simulated_image_grid.clear();
      this->image_delegate->clean_best();
      clear();
    }
  }
}

void TDMap_Table::connect_thickness_range_number_samples_changes( const TreeItem* item, int item_changes_column ){
  connect( item, SIGNAL(dataChanged( int )), this, SLOT( update_RowCount_from_thickness_range_number_samples(int) ) );
  _treeitem_thickness_range_number_samples_watch_col = item_changes_column;
}

void TDMap_Table::connect_defocus_range_number_samples_changes( const TreeItem* item, int item_changes_column ){
  connect( item, SIGNAL(dataChanged( int )), this, SLOT( update_ColumnCount_from_defocus_range_number_samples(int) ) );
  _treeitem_defocus_range_number_samples_watch_col = item_changes_column;
}

void TDMap_Table::update_RowCount_from_thickness_range_number_samples( int signal_item_changed_column ){
  if ( signal_item_changed_column == _treeitem_thickness_range_number_samples_watch_col ){
    if( core_tdmap->get_flag_slice_samples() ){
    int new_RowCount = core_tdmap->get_slice_samples();
    RowCount = new_RowCount;
    update_row_size();
    std::cout << "new_RowCount " << new_RowCount << std::endl;
    clear();
    }
  }
}

void TDMap_Table::update_column_size(){
  int leftM,rightM,topM,bottomM;
  this->getContentsMargins(&leftM,&topM,&rightM,&bottomM);
  const int header_size = this->verticalHeader()->width();
  std::cout << "VerticalHeaderSize" << header_size << std::endl;
  const int total_width = this->width() -  header_size - leftM - rightM - 2* (this->frameWidth());
  std::cout << "total_width" << total_width << std::endl;
  ColumnSize = ColumnCount > 0 ? ( total_width / ColumnCount ) : total_width;
  std::cout << "adjusting tdmap cell width to << " << ColumnSize <<  "px to  fill a total of " << total_width << std::endl;
}

void TDMap_Table::update_row_size(){

  int leftM,rightM,topM,bottomM;
  this->getContentsMargins(&leftM,&topM,&rightM,&bottomM);

  const int header_size = this->horizontalHeader()->height();
  std::cout << "HorizontalHeaderSize" << header_size << std::endl;
  const int total_height = this->height() - header_size - topM - bottomM - 2* (this->frameWidth());
  std::cout << "total_height" << total_height << std::endl;

  RowSize = RowCount > 0 ? ( total_height / RowCount ) : total_height;
  std::cout << "adjusting tdmap cell height to << " << RowSize <<  "px to  fill a total of " << total_height << std::endl;
}

void TDMap_Table::update_ColumnCount_from_defocus_range_number_samples( int signal_item_changed_column ){
  if ( signal_item_changed_column == _treeitem_defocus_range_number_samples_watch_col ){
if( core_tdmap->get_flag_defocus_samples() ){
    int new_ColumnCount = core_tdmap->get_defocus_samples();
    ColumnCount = new_ColumnCount;
    update_column_size();
    std::cout << "new_ColumnCount " << new_ColumnCount << std::endl;
    clear();
  }
  }
}

void TDMap_Table::set_simulated_images_grid( std::vector< std::vector<cv::Mat> > image_grid ){
  simulated_image_grid = image_grid;
  _flag_simulated_image_grid = true;
  clear();
}

void TDMap_Table::update_headers(){
  double _defocus_lower = 0.0f;
  double _defocus_period = 0.0f;
  int _thickness_lower_slice = 0;
  int _thickness_period_slice = 0;
  if( _flag_core_tdmap ){
    if( core_tdmap->get_flag_defocus_lower_bound() ){
      _defocus_lower = core_tdmap->get_defocus_lower_bound();
    }
    if(  core_tdmap->get_flag_defocus_period() ){
      _defocus_period = core_tdmap->get_defocus_period();
    }

    if( core_tdmap->get_flag_nm_lower_bound() ){
      _thickness_lower_slice = core_tdmap->get_slices_lower_bound();
    }
    if( core_tdmap->get_flag_slice_period() ){
      _thickness_period_slice = core_tdmap->get_slice_period();
    }
    if( core_tdmap->get_flag_defocus_samples() ){
      ColumnSize = core_tdmap->get_defocus_samples();
    }
    if( core_tdmap->get_flag_slice_samples() ){
      RowSize = core_tdmap->get_slice_samples();
    }
  }

  for (int i = 0; i < ColumnCount; ++i) {
    setItemDelegateForColumn(i, image_delegate);
    QTableWidgetItem *item = new QTableWidgetItem;
    double _at_defocus = _defocus_lower + ( _defocus_period * i );
    item->setText( QString::number( _at_defocus ) + QString( " nm" )  );
    setHorizontalHeaderItem(i, item);
  }
  std::vector <double> _local_accum_nm_slice_vec;
  bool use_accum_nm = false;
  if ( core_tdmap != nullptr ){
    if( core_tdmap->get_flag_slice_params_accum_nm_slice_vec() ){
      use_accum_nm = true;
      _local_accum_nm_slice_vec = core_tdmap->get_slice_params_accum_nm_slice_vec();
    }
  }
  for (int i = 0; i < RowCount; ++i) {
    setItemDelegateForRow(i, image_delegate);
    QTableWidgetItem *item = new QTableWidgetItem;
    const int at_slice = round( _thickness_lower_slice + _thickness_period_slice * ( i )  );
    QString legend;
    if( use_accum_nm ){
      const double slice_thickness_nm = _local_accum_nm_slice_vec.at(at_slice-1);
      legend = QString::number( slice_thickness_nm ) + QString( "nm  #" ) + QString::number( at_slice );
    }
    else {
      legend = QString::number( at_slice );
    }
    item->setText( legend );
    setVerticalHeaderItem(i, item);
  }
  this->resizeColumnsToContents();
  this->resizeRowsToContents();
  update_row_size();
  update_column_size();
}

void TDMap_Table::clear(){
  setRowCount(RowCount);
  setColumnCount(ColumnCount);
  _number_calculated_cells = RowCount * ColumnCount;
  update_headers();
  if( ( _number_drawed_cells != _number_calculated_cells ) || ( ! _flag_created_cells ) ){
    std::cout << " creating cells " << std::endl;
    create_cells();
  }
  std::cout << " updating cells " << std::endl;
  update_cells();
  this->resizeColumnsToContents();
  this->resizeRowsToContents();
}

void TDMap_Table::create_cells(){

  for (int row = 0; row < RowCount; ++row) {
    for (int col = 0; col < ColumnCount; ++col) {
      CvImageCellWidget *cell_widget  = new CvImageCellWidget(  );
      cell_widget->setMaximumSize( QSize(ColumnSize, RowSize) );
      cell_widget->set_container_size( ColumnSize, RowSize );

      //cell_widget->setStyleSheet("background-color:red;");
      this->setCellWidget(row, col, cell_widget);
      this->setItem(row, col, new QTableWidgetItem());//used to find it
    }
  }
  _number_drawed_cells = RowCount * ColumnCount;
  _flag_created_cells = true;
}

void TDMap_Table::update_cells(){
  if( _flag_created_cells ){
    if( _flag_simulated_image_grid ){
      cv::Point2i best_match_pos = core_tdmap->get_simgrid_best_match_position();
      for (int row = 0; row < RowCount; ++row) {
        std::vector<cv::Mat> simulated_image_row = simulated_image_grid.at(row);
        for (int col = 0; col < ColumnCount; ++col) {

          CvImageCellWidget *cell_widget  = new CvImageCellWidget(  );
          cell_widget->setMaximumSize( QSize(ColumnSize, RowSize) );
          cell_widget->set_container_size( ColumnSize, RowSize );
          cv::Mat full_image = simulated_image_row.at(col);
          cell_widget->setImage( full_image.clone() );
          cell_widget->fitToContainer();
          if( best_match_pos.x ==  row && best_match_pos.y == col ){
            cell_widget->set_best();
            this->image_delegate->set_best(  row, col );
          }
          this->setCellWidget(row,col, cell_widget);
          this->setItem(row, col, new QTableWidgetItem());//used to find it
        }
      }
      emit tdmap_best_match( best_match_pos.x, best_match_pos.y );
      setCurrentCell( 0,0 );
      emit cellClicked( 0,0 );
    }
    else{
      //only visualy update cells
      for (int row = 0; row < RowCount; ++row) {
        for (int col = 0; col < ColumnCount; ++col) {
          CvImageCellWidget *cell_widget  = new CvImageCellWidget(  );
          cell_widget->setMaximumSize( QSize( ColumnSize, RowSize ) );
          cell_widget->set_container_size( ColumnSize, RowSize );
          cell_widget->fitToContainer();
          this->setCellWidget(row,col, cell_widget);
          this->setItem(row, col, new QTableWidgetItem());//used to find it
        }
      }
    }
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
