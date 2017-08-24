#ifndef SRC_CVTDMAPCELLIMAGEFRAMEDELEGATE_H__
#define SRC_CVTDMAPCELLIMAGEFRAMEDELEGATE_H__

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
#include "cv_image_cell_widget.h"


class QAbstractItemModel;
class QObject;
class QPainter;

class CvTDMapImageFrameDelegate : public QStyledItemDelegate
{
  Q_OBJECT
  public:
    CvTDMapImageFrameDelegate(int margin, QWidget *parent = 0) : QStyledItemDelegate(parent),  m_margin(margin) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool shouldBeBest(const QModelIndex &index) const;
    void set_best( int row, int col );
    void clean_best( );

  private:
    // margin between cells
    int m_margin;
    int _best_row;
    int _best_col;
    bool _best_defined = false;
};

#endif // SRC_CVTDMAPCELLIMAGEFRAMEDELEGATE_H__
