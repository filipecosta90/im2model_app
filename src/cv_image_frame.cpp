#include "cv_image_frame.h"

CvImageFrameWidget::CvImageFrameWidget(QWidget *parent ) : QWidget(parent) {
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
  contentsLayout = new QVBoxLayout;
  contentsLayout->addWidget(scrollArea);
  toolsLayout->addLayout(contentsLayout);
}

void CvImageFrameWidget::cleanRenderAreas(){
  image_widget->cleanRenderAreas();
  image_widget->update();
}

void CvImageFrameWidget::setImage(const cv::Mat& image) {
  image_widget->setImage(image);
  scrollArea->setWidget(image_widget);
  scrollArea->show();

  int leftM,rightM,topM,bottomM;
  this->getContentsMargins(&leftM,&topM,&rightM,&bottomM);

  const int _avail_width = scrollArea->width() - leftM - rightM - 2* (scrollArea->frameWidth());
  const int _avail_heigth = scrollArea->height() - topM - bottomM - 2* (scrollArea->frameWidth());
  image_widget->set_container_window_size( _avail_width , _avail_heigth );
  image_widget->update();
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

void CvImageFrameWidget::setImageWidget( CVImageWidget* widget ){
  image_widget = widget;
  image_widget->update();
}

