#pragma once
#include <QWidget>
#include <QtWidgets>
#include <QAbstractItemView>
#include <QImage>
#include <QPainter>
#include <opencv2/opencv.hpp>
#include <QFrame>
#include <QBoxLayout>
#include <QTableView>

#include "cv_image_widget.h"
#include "cv_image_frame.h"

class CvImageFrameTableWidget : public CvImageFrameWidget
{
  Q_OBJECT
  public:
    explicit CvImageFrameTableWidget(QWidget *parent = 0) : CvImageFrameWidget(parent)
  {
    table_view = new QTableView;
    //disable editing
    table_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_view->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    table_view->resizeRowsToContents();
    table_view->setMaximumHeight(0);

    contentsLayout->addWidget(table_view);
  }
    public slots:

      void setModel(QAbstractItemModel* model ){
        table_view->setModel(model);
        table_view->resizeRowsToContents();
        int row_heigth = table_view->rowHeight(0) * model->rowCount();
        int header_heigth = table_view->horizontalHeader()->height();
        table_view->setMaximumHeight(row_heigth + header_heigth);
        contentsLayout->update();
        table_view->show();
      }

  protected:
    QTableView *table_view;

};

