/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#include "cv_image_frame.h"

CvImageFrameWidget::CvImageFrameWidget(QWidget *parent ) : QWidget(parent) {
  parentWidget = parent;
  image_widget = new CVImageWidget(parent);
  scrollArea = new QScrollArea(this);

  scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
  toolsLayout = new QBoxLayout(QBoxLayout::TopToBottom,this);
  //set margins to zero so the toolbar touches the widget's edges
  toolsLayout->setContentsMargins(0, 0, 0, 0);

  toolbar = new QToolBar(this);
  toolbar->addAction("Actual size", image_widget, SLOT(normalSize()) );
  toolbar->addAction("Fit" , image_widget, SLOT(fitToWindow()) );
  toolbar->addAction("Zoom in" ,image_widget, SLOT(zoomIn()) );
  toolbar->addAction("Zoom out", image_widget, SLOT(zoomOut()) );

  const QIcon selectionRectangleIcon = QIcon::fromTheme("rectangle-selection", QIcon(":/Icons/ToolBoxSelectionRectangleIcon"));
  rectSelectAct = new QAction( selectionRectangleIcon, tr("&Rectangle selection"), this);
  rectSelectAct->setStatusTip(tr("Create rectangular selections"));

  const QIcon selectionFreeIcon = QIcon::fromTheme("free-selection", QIcon(":/Icons/ToolBoxSelectionFreeIcon"));
  freeSelectAct = new QAction( selectionFreeIcon, tr("&Free selection"), this);
  freeSelectAct->setStatusTip(tr("Create free selections"));

  const QIcon selectionPathIcon = QIcon::fromTheme("path-selection", QIcon(":/Icons/ToolBoxSelectionPathIcon"));
  pathSelectAct = new QAction( selectionPathIcon, tr("&Path selection"), this);
  pathSelectAct->setStatusTip(tr("Create path selections"));

  statistical_rectSelectAct = new QAction( selectionRectangleIcon, tr("&Mean and std. dev. from rectangle selection"), this);
  statistical_rectSelectAct->setStatusTip(tr("Extract mean and standard deviation features from rectangular selection"));

  toolsMenu = new QMenu("Tools");
  selectionMenu = new QMenu("Selection");
  dataCollectionMenu = new QMenu("Data collection");
  statisticalMenu = new QMenu("Statistical");

  selectionMenu->addAction(rectSelectAct);
  selectionMenu->addAction(freeSelectAct);
  selectionMenu->addAction(pathSelectAct);

  toolsMenu->addMenu(selectionMenu);
  toolsMenu->addMenu(dataCollectionMenu);
  dataCollectionMenu->addMenu(statisticalMenu);
  statisticalMenu->addAction(statistical_rectSelectAct);

  //statisticalMenu->addAction(pathSelectAct);

  toolsButton = new QToolButton();
  toolsButton->setPopupMode(QToolButton::MenuButtonPopup);
  toolsButton->setMenu(toolsMenu);
  toolsButton->setText(toolsMenu->title());

  toolbar->addWidget(toolsButton);

  // set toolsMenu not visible by default
  toolbar->actions().at(4)->setVisible(false);

  toolsLayout->addWidget(toolbar);

  //use a different layout for the contents so it has normal margins
  contentsLayout = new QVBoxLayout;
  contentsLayout->addWidget(scrollArea);
  toolsLayout->addLayout(contentsLayout);
}

/* Loggers */
bool CvImageFrameWidget::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
  BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification, "Application logger setted for CvImageFrameWidget class." );
  return true;
}

void CvImageFrameWidget::cleanRenderAreas(){
  image_widget->cleanRenderAreas();
  image_widget->update();
}

void CvImageFrameWidget::enableRectangleSelection(){
  // set toolsMenu visible
  toolbar->actions().at(4)->setVisible(true);
  connect( rectSelectAct, SIGNAL(triggered()), image_widget, SLOT(startRectangleSelection()) );
  connect( image_widget, SIGNAL(selectionRectangleChanged(QRect)), this, SLOT(emit_selectionRectangleChanged(QRect)) );

  connect( statistical_rectSelectAct, SIGNAL(triggered()), image_widget, SLOT(startStatisticalRectangleSelection()) );
  connect( image_widget, SIGNAL(selectionStatisticalRectangleChanged(QRect)), this, SLOT(emit_selectionStatisticalRectangleChanged(QRect)) );
}

void CvImageFrameWidget::emit_selectionRectangleChanged(QRect rect){
  emit selectionRectangleChanged( rect );
}

void CvImageFrameWidget::emit_selectionStatisticalRectangleChanged(QRect rect){
  emit selectionStatisticalRectangleChanged( rect );
}

void CvImageFrameWidget::setImage(const cv::Mat& image, int layer_number, QString ImageDescription,  cv::Point2i margin_point ) {
  image_widget->setImage( image, layer_number, ImageDescription, margin_point );
  scrollArea->setWidget(image_widget);
  scrollArea->show();

  int leftM,rightM,topM,bottomM;
  this->getContentsMargins( &leftM,&topM,&rightM,&bottomM );

  const int _avail_width = scrollArea->width() - leftM - rightM - 2* (scrollArea->frameWidth());
  const int _avail_heigth = scrollArea->height() - topM - bottomM - 2* (scrollArea->frameWidth());
  std::cout << "setting _avail_width " << _avail_width <<  " _avail_heigth " << _avail_heigth << std::endl;
  image_widget->set_container_window_size( _avail_width , _avail_heigth );
  image_widget->update();
}

int CvImageFrameWidget::addImageLayer( const cv::Mat& image ){
  int result = -1;
  if( image_widget != nullptr ){
    result = image_widget->addImageLayer(image);
    image_widget->update();
  }
  return result;
}

int CvImageFrameWidget::get_image_layer_alpha_channel( int layer_number ){
  int result = 255;
  if( image_widget != nullptr ){
    result = image_widget->get_image_layer_alpha_channel(layer_number);
    image_widget->update();
  }
  return result;
}


bool CvImageFrameWidget::set_image_layer_alpha_channel( int layer_number, int value ){
  bool result = false;
  if( image_widget != nullptr ){
    result = image_widget->set_image_layer_alpha_channel(layer_number, value);
    image_widget->update();
  }
  return result;
}

int CvImageFrameWidget::get_renderPoints_alpha_channels_map( QString key ){
  int result = 255;
  if( image_widget != nullptr ){
    result = image_widget->get_renderPoints_alpha_channels_map( key );
    image_widget->update();
  }
  return result;
}

bool CvImageFrameWidget::set_renderPoints_alpha_channels_map( QString key, int value ){
  bool result = false;
  if( image_widget != nullptr ){
    result = image_widget->set_renderPoints_alpha_channels_map( key, value );
    image_widget->update();
  }
  return result;
}

bool CvImageFrameWidget::set_renderPoints_alpha_channels_map_group( QString group_key, int value ){
  bool result = false;
  if( image_widget != nullptr ){
    result = image_widget->set_renderPoints_group_alpha_channels_map( group_key, value );
    image_widget->update();
  }
  return result;
}

bool CvImageFrameWidget::addRenderPoints_key_to_group( QString key_string, QString group_key ){
  bool result = false;
  if( image_widget != nullptr ){
    image_widget->addRenderPoints_key_to_group( key_string, group_key );
    image_widget->update();
    result = true;
  }
  return result;
}

bool CvImageFrameWidget::set_render_point_selected_state( QString key, bool value ){
  bool result = false;
  if( image_widget != nullptr ){
    result = image_widget->set_render_point_selected_state( key, value );
    image_widget->update();
  }
  return result;
}

void CvImageFrameWidget::addShapeRect( cv::Rect _rectangle, int pen_width, QString shape_description ){
  image_widget->addRect( _rectangle, pen_width , shape_description);
  image_widget->update();
}

void CvImageFrameWidget::addShapeRect( cv::Rect _rectangle, int pen_width, cv::Vec3b pen_color, QString shape_description ){
  image_widget->addRect( _rectangle, pen_width , pen_color , shape_description);
  image_widget->update();
}

void CvImageFrameWidget::addShapePolygon( std::vector<cv::Point2i> polygon , cv::Point2i top_left,  int pen_width, QString shape_description ){
  image_widget->addShapePolygon( polygon, top_left, pen_width , shape_description);
  image_widget->update();
}

void CvImageFrameWidget::addShapePolygon( std::vector<cv::Point2i> polygon , cv::Point2i top_left,  int pen_width, cv::Vec3b pen_color, QString shape_description ){
  image_widget->addShapePolygon( polygon, top_left, pen_width , pen_color , shape_description);
  image_widget->update();
}

void CvImageFrameWidget::addRenderPoints( std::vector<cv::Point2i> points, int penWidth, std::vector<cv::Vec3b> penColors, QString description, cv::Point2i margin_point ){
  image_widget->addRenderPoints( points , penWidth, penColors, description, margin_point );
  image_widget->update();
}

void CvImageFrameWidget::addRenderPoints( std::vector<cv::Point2i> points, int penWidth, cv::Vec3b penColor, QString description, cv::Point2i margin_point ){
  image_widget->addRenderPoints( points , penWidth, penColor, description, margin_point );
  image_widget->update();
}

void CvImageFrameWidget::setImageWidget( CVImageWidget* widget ){
  image_widget = widget;
  image_widget->update();
}
