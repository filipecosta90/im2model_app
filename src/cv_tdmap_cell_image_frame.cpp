/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#include "cv_tdmap_cell_image_frame.h"

CvTDMapImageFrame::CvTDMapImageFrame( ) {
  image_widget = new CVImageWidget();
  scrollArea = new QScrollArea();

  scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
  toolsLayout = new QBoxLayout(QBoxLayout::TopToBottom);
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

/* Loggers */
bool CvTDMapImageFrame::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
  BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification, "Application logger setted for CvTDMapImageFrame class." );
  return true;
}

CvTDMapImageFrame::CvTDMapImageFrame( QWidget * parent ) {
  image_widget = new CVImageWidget( parent );
  scrollArea = new QScrollArea( parent );

  scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
  toolsLayout = new QBoxLayout(QBoxLayout::TopToBottom, parent );
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

bool CvTDMapImageFrame::_is_image_setted(){
  return _flag_image_setted;
}

void CvTDMapImageFrame::setImage(const cv::Mat& image) {
  image_widget->set_container_window_size( scrollArea->width(), scrollArea->height() );
  image_widget->setImage(image);
  scrollArea->setWidget(image_widget);
  scrollArea->show();
  _flag_image_setted = true;
}

void CvTDMapImageFrame::setImageWidget( CVImageWidget* widget ){
  image_widget = widget;
}
