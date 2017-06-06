#ifndef CELL_DELEGATE_HPP
#define CELL_DELEGATE_HPP

#include <QWidget>
#include <QtWidgets>
#include <QAbstractItemView>
#include <QImage>
#include <QPainter>
#include <opencv2/opencv.hpp>
#include <QFrame>
#include <QBoxLayout>
#include <QAbstractItemDelegate>
#include <QModelIndex>
#include <QSize>

#include <QStyledItemDelegate>

#include "cv_tdmap_cell_image_frame.h"
#include "cv_image_frame.h"

class QAbstractItemModel;
class QObject;
class QPainter;

class CvTDMapImageFrameDelegate : public QStyledItemDelegate
{
  Q_OBJECT
  public:
    CvTDMapImageFrameDelegate(int margin, QWidget *parent = 0) : QStyledItemDelegate(parent),  m_margin(margin) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

  private:
      // margin between cells
      int m_margin;
};

#endif //CELL_DELEGATE_HPP
