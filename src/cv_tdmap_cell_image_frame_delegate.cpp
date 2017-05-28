#include <QWidget>
#include <QtWidgets>
#include <QAbstractItemView>
#include <QImage>
#include <QPainter>
#include <opencv2/opencv.hpp>
#include <QFrame>
#include <QBoxLayout>


#include "cv_tdmap_cell_image_frame_delegate.h"
#include "cv_tdmap_cell_image_frame.h"
#include "cv_image_frame.h"

void CvTDMapImageFrameDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  if (index.data().canConvert<CvTDMapImageFrame>()) {
    CvTDMapImageFrame _tdmap_image_frame = qvariant_cast<CvTDMapImageFrame>(index.data());
    std::cout << "inside paint of CvTDMapImageFrame" << std::endl;
    std::cout << "_is_image_setted " << _tdmap_image_frame._is_image_setted() <<  std::endl;

    if (option.state & QStyle::State_Selected){
      painter->fillRect(option.rect, option.palette.highlight());
      // painter->fillRect(option.rect, option.palette.highlight());
    }
    //_tdmap_image_frame.paint(painter);

  } else {
    //QStyledItemDelegate::paint(painter, option, index);
  }
}

QSize CvTDMapImageFrameDelegate::sizeHint(const QStyleOptionViewItem &option,
    const QModelIndex &index) const {
  if ( index.data().canConvert<CvTDMapImageFrame>() ) {
    CvTDMapImageFrame _tdmap_image_frame = qvariant_cast<CvTDMapImageFrame>(index.data());
  } else {
    return QStyledItemDelegate::sizeHint(option, index);
  }
}

QWidget *CvTDMapImageFrameDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
  if (index.data().canConvert<CvTDMapImageFrame>()) {
    QWidget* editor = 0;
    return editor;
  } else {
    return QStyledItemDelegate::createEditor(parent, option, index);
  }

}

void CvTDMapImageFrameDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
  if (index.data().canConvert<CvTDMapImageFrame>()) {

  } else {
    QStyledItemDelegate::setEditorData(editor, index);
  }

}

void CvTDMapImageFrameDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
  if (index.data().canConvert<CvTDMapImageFrame>()) {

  } else {
    QStyledItemDelegate::setModelData(editor, model, index);
  }

}
