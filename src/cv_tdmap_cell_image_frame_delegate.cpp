#include "cv_tdmap_cell_image_frame_delegate.h"
#include "cv_tdmap_cell_image_frame.h"
#include "cv_image_frame.h"
#include "cv_image_cell_widget.h"

void CvTDMapImageFrameDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
  // save the painter's state
  painter->save();

  painter->setRenderHint(QPainter::Antialiasing);

  if (option.state & QStyle::State_Selected){
    painter->setPen(QPen(QColor(255,0,0),1));
    painter->setBrush(QBrush(QColor(255, 0, 0)));
  }

  if ( shouldBeBest(index) ){
    painter->setPen(QPen(QColor(0,255,0),1));
    painter->setBrush(QBrush(QColor(0, 255, 0)));
  }

  QStyleOptionViewItem itemOption(option);
  painter->drawRect(itemOption.rect );

  // restore the painter's state
  painter->restore();
}

bool CvTDMapImageFrameDelegate::shouldBeBest(const QModelIndex &index) const {
  bool result = false;
  if( (index.row() == _best_row) && (index.column() == _best_col) && (_best_defined) ){
    result = true;
  }
  return result;
}

void CvTDMapImageFrameDelegate::set_best( int row, int col ){
  _best_row = row;
  _best_col = col;
  _best_defined = true;
}

