/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#ifndef SRC_CVIMAGECELLWIDGET_H__
#define SRC_CVIMAGECELLWIDGET_H__

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

/* Loggers */
bool set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
  BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification, "Application logger setted for CvImageCellWidget class." );
  return true;
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

        if( _flag_logger ){
      std::stringstream message;
      message << "TDMap_Table::setImage";
      ApplicationLog::severity_level _log_type = ApplicationLog::normal;
      BOOST_LOG_FUNCTION();
      logger->logEvent( _log_type , message.str() );
    }
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

  private:
        /* Loggers */
    ApplicationLog::ApplicationLog* logger = nullptr;
    bool _flag_logger = false;
    // QToolBar *toolbar;
};

#endif
