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

  QStyleOptionViewItem itemOption(option);
  painter->drawRect(itemOption.rect );

  // restore the painter's state
  painter->restore();

}
