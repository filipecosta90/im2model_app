#include "cv_tdmap_cell_image_frame_delegate.h"
#include "cv_tdmap_cell_image_frame.h"
#include "cv_image_frame.h"
#include "cv_image_cell_widget.h"

void CvTDMapImageFrameDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
/*
  CvImageCellWidget *item = static_cast<CvImageCellWidget*>(index.internalPointer());
  QStyleOptionViewItem itemOption(option);

 // std::cout << "Rectangle size: w " << option.rect.width() << " h " << option.rect.height() << std::endl;

  // Make the 'drawing rectangle' smaller.
 // itemOption.rect.adjust(m_margin, m_margin, -m_margin, -m_margin);
 // std::cout << "\t\tAdjusted size: w " << itemOption.rect.width() << " h " << itemOption.rect.height() << std::endl;


  if ( option.state & QStyle::State_Selected ){
   // std::cout << "painting special TDMap selected rectangle" << std::endl;

    // save the painter's state
    painter->save();

    QPen pen;  // creates a default pen

    pen.setStyle(Qt::SolidLine);
    pen.setWidth(m_margin);
    pen.setBrush(Qt::red);
    pen.setCapStyle(Qt::SquareCap);
    pen.setJoinStyle(Qt::RoundJoin);

    // set the painter's pen & brush
    painter->setPen(pen);

    painter->drawRect(option.rect);

    // restore the painter's state
    painter->restore();

  }
*/
  QStyledItemDelegate::paint(painter, option, index);

}

QSize CvTDMapImageFrameDelegate::sizeHint(const QStyleOptionViewItem &option,
    const QModelIndex &index) const {
  if ( index.data().canConvert<CvTDMapImageFrame>() ) {
    CvTDMapImageFrame _tdmap_image_frame = qvariant_cast<CvTDMapImageFrame>(index.data());
  } else {
   // return QStyledItemDelegate::sizeHint(option, index);
  }
}

QWidget *CvTDMapImageFrameDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
  if (index.data().canConvert<CvTDMapImageFrame>()) {
    QWidget* editor = 0;
    return editor;
  } else {
   // return QStyledItemDelegate::createEditor(parent, option, index);
  }

}

void CvTDMapImageFrameDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
  if (index.data().canConvert<CvTDMapImageFrame>()) {

  } else {
  //  QStyledItemDelegate::setEditorData(editor, index);
  }
}

void CvTDMapImageFrameDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
  if (index.data().canConvert<CvTDMapImageFrame>()) {

  } else {
  //  QStyledItemDelegate::setModelData(editor, model, index);
  }

}
