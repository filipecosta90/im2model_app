#pragma once
#include <QWidget>
#include <QtWidgets>
#include <QAbstractItemView>
#include <QImage>
#include <QPainter>
#include <opencv2/opencv.hpp>
#include <QFrame>
#include <QBoxLayout>

#include "cv_image_widget.h"

class CvImageCellWidget : public QWidget
{
  Q_OBJECT
  public:
    explicit CvImageCellWidget(QWidget *parent = 0) : QWidget(parent)
  {
    parentWidget = parent;
    image_widget = new CVImageWidget(parent);
    scrollArea = new QScrollArea(this);

    scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    toolsLayout = new QBoxLayout(QBoxLayout::TopToBottom,this);
    //set margins to zero so the toolbar touches the widget's edges
    toolsLayout->setContentsMargins(0, 0, 0, 0);

    //use a different layout for the contents so it has normal margins
    contentsLayout = new QHBoxLayout;
    contentsLayout->addWidget(scrollArea);
    toolsLayout->addLayout(contentsLayout);
  }

    QSize sizeHint() const { return image_widget->sizeHint(); }
    QSize minimumSizeHint() const { return image_widget->minimumSizeHint(); }

    public slots:

      void setImage(const cv::Mat& image) {
        image_widget->setImage(image);
        scrollArea->setWidget(image_widget);
        scrollArea->show();
        image_widget->set_container_window_size(scrollArea->width(), scrollArea->height());
      }

    void setImageWidget( CVImageWidget* widget ){
      image_widget = widget;
    }
  protected:
    CVImageWidget *image_widget;
    QWidget *parentWidget;
    QScrollArea* scrollArea;
    QHBoxLayout *contentsLayout;
    QBoxLayout *toolsLayout;
    // QToolBar *toolbar;
};

