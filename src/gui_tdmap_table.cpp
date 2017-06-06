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
    update_row_size();
    std::cout << "new_RowCount " << new_RowCount << std::endl;
    clear();
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
    int new_ColumnCount = core_tdmap->get_defocus_range_number_samples();
    ColumnCount = new_ColumnCount;
    update_column_size();
    std::cout << "new_ColumnCount " << new_ColumnCount << std::endl;
    clear();
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
    std::cout << " core_tdmap->_is_nx_simulated_horizontal_samples_defined()  " << core_tdmap->_is_nx_simulated_horizontal_samples_defined()  << std::endl;
    if( core_tdmap->_is_nx_simulated_horizontal_samples_defined() ){
      ColumnSize = core_tdmap->get_nx_simulated_horizontal_samples();
      std::cout << " Column size setted to " << ColumnSize << std::endl;
    }
    std::cout << " core_tdmap->_is_ny_simulated_vertical_samples_defined()  " << core_tdmap->_is_ny_simulated_vertical_samples_defined()  << std::endl;

    if( core_tdmap->_is_ny_simulated_vertical_samples_defined() ){
      RowSize = core_tdmap->get_ny_simulated_vertical_samples();
      std::cout << " RowSize size setted to " << RowSize << std::endl;
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
    if( core_tdmap->_is_celslc_accum_nm_slice_vec_defined() ){
      use_accum_nm = true;
      _local_accum_nm_slice_vec = core_tdmap->get_celslc_accum_nm_slice_vec();
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
  setRowCount(0);
  setColumnCount(0);
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

Q_DECLARE_METATYPE(CvImageCellWidget*)

  void TDMap_Table::create_cells(){

    for (int row = 0; row < RowCount; ++row) {
      for (int col = 0; col < ColumnCount; ++col) {
        CvImageCellWidget *cell_widget  = new CvImageCellWidget(  );
        cell_widget->setMaximumSize( QSize(ColumnSize, RowSize) );
        this->setCellWidget(row, col, cell_widget);
        this->setItem(row, col, new QTableWidgetItem());//used to find it

      }
    }
    _number_drawed_cells = RowCount * ColumnCount;
    _flag_created_cells = true;
  }

void TDMap_Table::update_cells(){
  if( _flag_simulated_image_grid && _flag_created_cells ){

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
        if( best_match_pos.x ==  col && best_match_pos.y == row ){
          std::cout << "setting best position "<< best_match_pos << std::endl;
          cell_widget->set_best();
        }
        this->setCellWidget(row,col, cell_widget);
        this->setItem(row, col, new QTableWidgetItem());//used to find it
      }
    }
    setCurrentCell(best_match_pos.x, best_match_pos.y);
    emit tdmap_best_match( best_match_pos.x, best_match_pos.y );
    setCurrentCell(0, 0);
    emit cellClicked(0,0);
  }
  /* else{
     setCurrentCell(0, 0);
     emit cellClicked(0,0);
     }*/
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
