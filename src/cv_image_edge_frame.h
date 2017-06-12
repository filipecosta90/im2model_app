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

class CvImageEdgeFrame : public QWidget
{
  Q_OBJECT
  public:
    explicit CvImageEdgeFrame(QWidget *parent = 0) : QWidget(parent)
  {
    parentWidget = parent;
    image_widget = new CVImageWidget(parent);
    scrollArea = new QScrollArea(this);
table_view = new QTableView;
//disable editing
table_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
table_view->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
table_view->resizeRowsToContents();
table_view->setMaximumHeight(0);

    scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    toolsLayout = new QBoxLayout(QBoxLayout::TopToBottom,this);
    //set margins to zero so the toolbar touches the widget's edges
    toolsLayout->setContentsMargins(0, 0, 0, 0);

    toolbar = new QToolBar;
    toolbar->addAction("Normal size", image_widget, SLOT(normalSize()) );
    toolbar->addAction("Fit" , image_widget, SLOT(fitToWindow()) );
    toolbar->addAction("Zoom in" ,image_widget, SLOT(zoomIn()) );
    toolbar->addAction("Zoom out", image_widget, SLOT(zoomOut()) );
    toolsLayout->addWidget(toolbar);

    //use a different layout for the contents so it has normal margins
    contentsLayout = new QVBoxLayout;
    contentsLayout->addWidget(scrollArea);
    contentsLayout->addWidget(table_view);
    toolsLayout->addLayout(contentsLayout);
  }
    public slots:

      void setImage(const cv::Mat& image) {
        image_widget->setImage(image);
        scrollArea->setWidget(image_widget);
        scrollArea->show();
        image_widget->set_container_window_size(scrollArea->width(), scrollArea->height());
      }

      void setModel(QAbstractItemModel* model ){
          table_view->setModel(model);
          table_view->resizeRowsToContents();
          int row_heigth = table_view->rowHeight(0) * model->rowCount();
          int header_heigth = table_view->horizontalHeader()->height();
          table_view->setMaximumHeight(row_heigth + header_heigth);
          contentsLayout->update();
          table_view->show();
      }

    void setImageWidget( CVImageWidget* widget ){
      image_widget = widget;
    }

  protected:
    CVImageWidget *image_widget;
    QWidget *parentWidget;
    QScrollArea* scrollArea;
    QVBoxLayout *contentsLayout;
    QBoxLayout *toolsLayout;
    QToolBar *toolbar;
    QTableView *table_view;

};

