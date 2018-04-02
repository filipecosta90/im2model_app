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
#include <QFocusEvent>

#include "cv_image_widget.h"

class CvImageCellWidget : public QWidget
{
  Q_OBJECT
public:
  explicit CvImageCellWidget(QWidget *parent = 0);

/* Loggers */
  bool set_application_logger( ApplicationLog::ApplicationLog* app_logger );
  void add_onfocus_functor( boost::function<void(QFocusEvent *event)> );

  void set_best();

  bool _is_best();

  void set_container_size( int _width, int _heigth );

  public slots:

  void setImage(const cv::Mat& image);

  void fitToContainer();

  void setImageWidget( CVImageWidget* widget );

protected:
    // void paintEvent(QPaintEvent *event) override;

//  TBD  void keyReleaseEvent(QKeyEvent *event); 
    void focusInEvent(QFocusEvent *event);
 // TBD   void focusOutEvent(QFocusEvent *event);

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
  boost::function<void(QFocusEvent *event)> focus_functor;
    // QToolBar *toolbar;
};

#endif
