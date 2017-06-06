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
    explicit CvImageCellWidget(QWidget *parent = 0) :  QWidget(parent)
  {
    parentWidget = parent;
    image_widget = new CVImageWidget(parent);
    scrollArea = new QScrollArea(this);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    toolsLayout = new QBoxLayout(QBoxLayout::TopToBottom,this);
    //set margins to zero so the toolbar touches the widget's edges
    toolsLayout->setContentsMargins(0, 0, 0, 0);

    //use a different layout for the contents so it has normal margins
    contentsLayout = new QHBoxLayout;
    contentsLayout->addWidget(scrollArea);
    toolsLayout->addLayout(contentsLayout);
  }



    void set_best(){
      _flag_best = true;
    }

    bool _is_best(){
      return _flag_best;
    }

    void set_container_size( int _width, int _heigth ){
      _container_window_width = _width;
      _container_window_height = _heigth;
      scrollArea->resize(_container_window_width,_container_window_height);
    }

    public slots:

      void setImage(const cv::Mat& image) {
        image_widget->setImage(image);
        scrollArea->setWidget(image_widget);
        scrollArea->show();
        image_widget->set_container_window_size(scrollArea->width(), scrollArea->height());
        _image_set = true;
      }

    void fitToContainer(){
      image_widget->set_container_window_size(scrollArea->width(), scrollArea->height());
      image_widget->fitToWindow();
      scrollArea->show();
    }

    void setImageWidget( CVImageWidget* widget ){
      image_widget = widget;
    }

  protected:
    // void paintEvent(QPaintEvent *event) override;

    CVImageWidget *image_widget;
    QWidget *parentWidget;
    QScrollArea* scrollArea;
    QHBoxLayout *contentsLayout;
    QBoxLayout *toolsLayout;

    bool _flag_best = false;
    bool _image_set = false;
    bool _flag_set_container_window_size = false;

    int _container_window_width = 0;
    int _container_window_height = 0;
    QSize _sz_hint;
    QSize _mn_sz_hint;
    // QToolBar *toolbar;
};

