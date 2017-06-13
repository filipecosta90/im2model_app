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

class CvImageFrameWidget : public QWidget
{
  Q_OBJECT
  public:
    explicit CvImageFrameWidget(QWidget *parent = 0) : QWidget(parent)
  {
    parentWidget = parent;
    image_widget = new CVImageWidget(parent);
    scrollArea = new QScrollArea(this);

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
    contentsLayout = new QHBoxLayout;
    contentsLayout->addWidget(scrollArea);
    toolsLayout->addLayout(contentsLayout);
  }
    public slots:

      void cleanRenderAreas(){
        image_widget->cleanRenderAreas();
        image_widget->update();
      }

    void setImage(const cv::Mat& image) {
      image_widget->setImage(image);
      scrollArea->setWidget(image_widget);
      scrollArea->show();

      int leftM,rightM,topM,bottomM;
      this->getContentsMargins(&leftM,&topM,&rightM,&bottomM);

      const int _avail_width = scrollArea->width() - leftM - rightM - 2* (scrollArea->frameWidth());
      const int _avail_heigth = scrollArea->height() - topM - bottomM - 2* (scrollArea->frameWidth());

      // std::cout << "total_width" << total_width << std::endl;

      image_widget->set_container_window_size( _avail_width , _avail_heigth );
      image_widget->update();

    }

    void addShapeRect( cv::Rect _rectangle, int pen_width ){
      image_widget->addRect( _rectangle, pen_width );
      image_widget->update();
    }

    void setImageWidget( CVImageWidget* widget ){
      image_widget = widget;
      image_widget->update();
    }
  protected:
    CVImageWidget *image_widget;
    QWidget *parentWidget;
    QScrollArea* scrollArea;
    QHBoxLayout *contentsLayout;
    QBoxLayout *toolsLayout;
    QToolBar *toolbar;
};

