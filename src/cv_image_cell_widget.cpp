#include "cv_image_cell_widget.h"

CvImageCellWidget::CvImageCellWidget(QWidget *parent ) : QWidget(parent) {
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

void CvImageCellWidget::add_onfocus_functor( boost::function<void(QFocusEvent *event)> functor ){
  focus_functor = functor;
  image_widget->add_onfocus_functor( functor );
     std::cout << " added add_onfocus_functor" << std::endl;
}

void CvImageCellWidget::focusInEvent ( QFocusEvent * event ) { 
  focus_functor( event );  
   std::cout << " received focusInEvent" << std::endl;
}

/* Loggers */
bool CvImageCellWidget::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
  BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification, "Application logger setted for CvImageCellWidget class." );
  return true;
}
void CvImageCellWidget::set_best(){
  _flag_best = true;
}

bool CvImageCellWidget::_is_best(){
  return _flag_best;
}

void CvImageCellWidget::set_container_size( int _width, int _heigth ){
  _container_window_width = _width;
  _container_window_height = _heigth;
  scrollArea->resize(_container_window_width,_container_window_height);
}

void CvImageCellWidget::setImage(const cv::Mat& image) {
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

void CvImageCellWidget::fitToContainer(){
  image_widget->set_container_window_size(scrollArea->width(), scrollArea->height());
  image_widget->fitToWindow();
  scrollArea->show();
}

void CvImageCellWidget::setImageWidget( CVImageWidget* widget ){
  image_widget = widget;
}

