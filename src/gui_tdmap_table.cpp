#include <QtGui>

#include "gui_tdmap_cell.h"
#include "gui_tdmap_table.h"
#include "td_map.hpp"

  TDMap_Table::TDMap_Table(QWidget *parent)
: QTableWidget(parent)
{
      table_parent = parent;
  autoRecalc = true;
  image_delegate = new CvImageDelegate(this);

  setItemPrototype(new TDMap_Cell);
  setSelectionMode(ContiguousSelection);
 // setItemDelegate(image_delegate);

  connect(this, SIGNAL(itemChanged(QTableWidgetItem *)),
      this, SLOT(somethingChanged()));
  clear();
}

void TDMap_Table::set_tdmap( TDMap* map ){
    core_tdmap = map;
  }

void TDMap_Table::connect_thickness_changes( const QObject* thickness_object ){
    std::cout << "connecting thickness object changes " << std::endl;
    connect(thickness_object, SIGNAL(dataChanged(const QModelIndex &,const QModelIndex &)), this, SLOT( update_map_size_defocus() ) );
}

void TDMap_Table::update_map_size_defocus(){
    std::cout << "updating map size defocus " << std::endl;
clear();
}

void TDMap_Table::clear()
{
  setRowCount(0);
  setColumnCount(0);
  setRowCount(RowCount);
  setColumnCount(ColumnCount);
  for (int i = 0; i < ColumnCount; ++i) {
    QTableWidgetItem *item = new QTableWidgetItem;
    item->setText(QString(QChar('A' + i)));
    setHorizontalHeaderItem(i, item);
  }
  setCurrentCell(0, 0);
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
 * The text() private function returns the text for a given cell. 
 * If cell() returns a null pointer, the TDMap_Cellis empty, so we return an empty string.
 * */
QString TDMap_Table::text(int row, int column) const
{
  TDMap_Cell*c = cell(row, column);
  if (c) {
    return c->text();
  } else {
    return "";
  }
}

QString TDMap_Table::formula(int row, int column) const
{
  TDMap_Cell*c = cell(row, column);
  if (c) {
    return c->formula();
  } else {
    return "";
  }
}

void TDMap_Table::setFormula(int row, int column,
    const QString &formula)
{
  TDMap_Cell*c = cell(row, column);
  if (!c) {
    c = new TDMap_Cell;
    setItem(row, column, c);
  }
  c->setFormula(formula);
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
 * The currentFormula() function returns the current cell's formula. 
 * It is called from MainWindow::updateStatusBar().
 * */

QString TDMap_Table::currentFormula() const
{
  return formula(currentRow(), currentColumn());
}

/*
 * The somethingChanged() private slot recalculates the whole TDMap_Table if 
 * "auto-recalculate" is enabled. It also emits the modified() signal.
 */
void TDMap_Table::somethingChanged()
{
  if (autoRecalc)
    recalculate();
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

/*
 * The recalculate() slot corresponds to Tools|Recalculate. It is also called automatically by TDMap_Table when necessary.
 * */
void TDMap_Table::recalculate()
{
  for (int row = 0; row < RowCount; ++row) {
    for (int column = 0; column < ColumnCount; ++column) {
      if (cell(row, column))
        cell(row, column)->setDirty();
    }
  }
  viewport()->update();
}

/*
 * The setAutoRecalculate() slot corresponds to Options|Auto-Recalculate. If the feature is being turned on, we recalculate the whole TDMap_Table immediately to make sure that it's up-to-date; afterward, recalculate() is called automatically from somethingChanged().
 * */
void TDMap_Table::setAutoRecalculate(bool recalc)
{
  autoRecalc = recalc;
  if (autoRecalc)
    recalculate();
}

bool TDMap_TableCompare::operator()(const QStringList &row1,
    const QStringList &row2) const
{
  for (int i = 0; i < KeyCount; ++i) {
    int column = keys[i];
    if (column != -1) {
      if (row1[column] != row2[column]) {
        if (ascending[i]) {
          return row1[column] < row2[column];
        } else {
          return row1[column] > row2[column];
        }
      }
    }
  }
  return false;
}

