/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#include "cv_image_widget.h"

CVImageWidget::CVImageWidget(QWidget *parent ) : QWidget(parent) , scaleFactor(1) {
  this->setFocusPolicy( Qt::StrongFocus );
  this->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(ShowContextMenu(const QPoint &)));
  connect(this, SIGNAL(MapPosClicked(QPoint)), this, SLOT(MapPosToObject(const QPoint &)));
}

void CVImageWidget::MapPosToObject(const QPoint &pos){
  // Handle global position
  std::cout << "MapPosToObject x "  << pos.x() << " y " << pos.y() << std::endl;
}

void CVImageWidget::add_onfocus_functor( boost::function<void(QFocusEvent *event)> functor ){
  focus_functor = functor;
  _flag_focus_functor = true;
     std::cout << " added add_onfocus_functor CVImageWidget" << std::endl;
}

void CVImageWidget::focusInEvent ( QFocusEvent * event ) {
  if( _flag_focus_functor ){
      focus_functor( event );  
  }
   std::cout << " received focusInEvent" << std::endl;
}

/* Loggers */
bool CVImageWidget::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
  BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification, "Application logger setted for CVImageWidget class." );
  return true;
}

QSize CVImageWidget::sizeHint() const {
  return largestImageLayerSize();
}

QSize CVImageWidget::minimumSizeHint() const {
  return largestImageLayerSize();
}

QSize CVImageWidget::largestImageLayerSize() const {
  QSize returnSize;
  returnSize.setHeight(0);
  returnSize.setWidth(0);
  for( int image_pos = 0; image_pos < images.size(); image_pos++ ){
    if( images_set[image_pos] ){
      const int total_height = images[image_pos].size().height() + margin_points[image_pos].y;
      const int total_width = images[image_pos].size().width() + margin_points[image_pos].x;
      if ( total_height > returnSize.height() ){
        returnSize.setHeight( total_height );
      }
      if ( total_width > returnSize.width() ){
        returnSize.setWidth( total_width );
      }
    }
  }
  return returnSize;
}

void CVImageWidget::set_container_window_size( const int width , const int height ){
  _container_window_width = width;
  _container_window_height = height;
}

void CVImageWidget::zoomIn(){
  scaleFactor += 0.10;
  updateImage();
}
void CVImageWidget::zoomOut(){
  scaleFactor -= 0.10;
  if(scaleFactor < 0.10 ){scaleFactor = 0.10;}
  updateImage();
}
void CVImageWidget::normalSize(){
  scaleFactor = 1;
  updateImage();
}

void CVImageWidget::fitToWindow(){
  int window_width = _container_window_width;
  int window_height = _container_window_height;
  cv::Size original_size(0,0);
  if( original_sizes.size() > 0 ){
    original_size = original_sizes[0];
  } 
  // prevent division by zero
  float w_factor = original_size.width > 0 ? ((float) window_width) / ((float) original_size.width ) : 0.0f;
  float h_factor = original_size.height > 0 ? ((float) window_height) / ((float) original_size.height ) : 0.0f;
  scaleFactor = std::min( w_factor , h_factor );
  updateImage();
}

void CVImageWidget::setImage( const cv::Mat& image, int layer_number, QString ImageDescription , cv::Point2i margin_point ){
  // Convert the image to the RGB888 format
  cv::Mat _tmp_original, _tmp_current;
  cv::Size original_size, current_size;

  switch (image.type()) {
    case cv::DataType<float>::type:
    {
      cv::Mat temp;
      image.convertTo(temp, cv::DataType<unsigned char>::type);
      cvtColor(temp, _tmp_original, CV_GRAY2RGBA );
      break;
    }
    case cv::DataType<int>::type:
    case cv::DataType<unsigned int>::type:
    {
      cv::Mat temp;
      image.convertTo(temp, cv::DataType<unsigned char>::type, 1.0f/65536.0f);
      cvtColor(temp, _tmp_original, CV_GRAY2RGBA );
      break;
    }
    case cv::DataType<char>::type:
    case cv::DataType<unsigned char>::type:
    {
      cvtColor(image, _tmp_original, CV_GRAY2RGBA);
      break;
    }
    case cv::DataType<short>::type:
    case cv::DataType<unsigned short>::type:
    {
      cv::Mat temp;
      image.convertTo(temp, cv::DataType<unsigned char>::type, 1.0f/255.0f);
      cvtColor(temp, _tmp_original, CV_GRAY2RGBA );
      break;
    }
    case CV_8UC3:
    cvtColor(image, _tmp_original, CV_BGR2RGBA );
    break;
  }

  original_size = image.size();
  current_size = original_size;
  // Assign OpenCV's image buffer to the QImage. Note that the bytesPerLine parameter
  // (http://qt-project.org/doc/qt-4.8/qimage.html#QImage-6) is 3*width because each pixel
  // has three bytes.
  QImage _qimage = QImage(_tmp_original.data, _tmp_original.cols, _tmp_original.rows, _tmp_original.cols*4, QImage::Format_RGBA8888  );

  for( int images_pos = images_set.size(); images_pos <= layer_number;images_pos++ ){
    images_set.push_back(false);
    alpha_channels.push_back( 255 );
    images.push_back(  QImage() );
    _tmp_originals.push_back(  cv::Mat() );
    _tmp_currents.push_back(  cv::Mat() );
    original_sizes.push_back(  cv::Size(0,0) );
    current_sizes.push_back(  cv::Size(0,0) );
    margin_points.push_back( cv::Point2i(0,0) );
  }
  images_set[layer_number] = true;
  alpha_channels[ layer_number ] = 255;
  images[ layer_number ] =  _qimage ;
  _tmp_originals[ layer_number ] = _tmp_original ;
  _tmp_currents[ layer_number ] =  _tmp_current ;
  original_sizes[ layer_number ] =  original_size ;
  current_sizes[ layer_number ] =  current_size ;
  //std::cout << "MARGIN POINT " << margin_point;
  margin_points[ layer_number] = margin_point;

  this->setFixedSize( largestImageLayerSize() );

}

void CVImageWidget::updateImage() {
  int largest_cols = 0;
  int largest_rows = 0;
  for( int pos = 0; pos < images.size(); pos++ ){

    if( images_set[pos] ){
      const cv::Mat _tmp_original = _tmp_originals[pos];
      cv::Mat _tmp_current;
      const cv::Size original_size = original_sizes[pos];
      cv::Size current_size = cv::Size(original_size.width * scaleFactor , original_size.height * scaleFactor);
    cv::resize(_tmp_original,_tmp_current,current_size);//resize image
    largest_cols =  ( largest_cols > _tmp_current.cols ) ? largest_cols : _tmp_current.cols;
    largest_rows =  ( largest_rows > _tmp_current.rows ) ? largest_rows : _tmp_current.rows;

    images[pos] = QImage(_tmp_current.data, _tmp_current.cols, _tmp_current.rows, _tmp_current.cols*4, QImage::Format_RGBA8888);
    current_sizes[pos]=current_size;
    _tmp_currents[pos]=_tmp_current;
  }
  
}
this->setFixedSize( largestImageLayerSize() );
}

void CVImageWidget::ShowContextMenu(const QPoint &pos){
  // Handle global position
  QPoint globalPos = this->mapToGlobal(pos);

  // Create menu and insert some actions
  QMenu areas_menu;

  for( int list_position = 0; list_position < renderAreas.size() ; list_position++ ){
    QString _path_description =renderAreas_description.at(list_position);
    const bool _path_visible = renderAreas_visible.at(list_position);

    QAction *pathAction = new QAction( _path_description );
    if(_path_visible){
      connect( pathAction, &QAction::triggered, [=](){ this->renderAreas_visible[list_position] = false; this->repaint(); });
    }
    else {
      connect( pathAction, &QAction::triggered, [=](){ this->renderAreas_visible[list_position] = true; this->repaint(); });
    }
    pathAction->setCheckable(true);
    pathAction->setChecked( _path_visible );
    areas_menu.addAction( pathAction );
  }

  // Show context menu at handling position
  areas_menu.exec(globalPos);
}

void CVImageWidget::addRect( cv::Rect _area_rect, int penWidth, QString description ){
  addRect(  _area_rect ,  penWidth, renderAreas_default_color, description );
}

void CVImageWidget::addRect( cv::Rect _area_rect, int penWidth, cv::Vec3b penColor, QString description ){
  renderAreas_description.push_back(description);
  renderAreas_visible.push_back(true);
  renderAreas_top_left.push_back(cv::Point2i( _area_rect.x, _area_rect.y ));
  QPainterPath rectPath;
  rectPath.addRect( 0, 0 , _area_rect.width, _area_rect.height );
  renderAreas.push_back(rectPath);
  renderAreas_penWidth.push_back( penWidth );
  renderAreas_penColor.push_back( QColor(penColor[0], penColor[1], penColor[2]) );
}

void CVImageWidget::addShapePolygon( std::vector<cv::Point2i> polygon , cv::Point2i top_left,  int penWidth , QString description ){
  addShapePolygon( polygon , top_left,  penWidth, renderAreas_default_color , description );
}

void CVImageWidget::addShapePolygon( std::vector<cv::Point2i> polygon , cv::Point2i top_left,  int penWidth, cv::Vec3b penColor , QString description ){
  renderAreas_description.push_back(description);
  renderAreas_visible.push_back(true);
  renderAreas_top_left.push_back(top_left);
  QPainterPath polyPath;
  QPolygon poly;

  for( int point_n = 0; point_n < polygon.size(); point_n++ ){
    const cv::Point2i _cv_point  = polygon.at(point_n);
    poly.push_back( QPoint(_cv_point.x, _cv_point.y) );
  }
  polyPath.addPolygon( poly );
  renderAreas.push_back( polyPath );
  renderAreas_penWidth.push_back( penWidth  );
  renderAreas_penColor.push_back( QColor(penColor[0], penColor[1], penColor[2]) );
}
void CVImageWidget::addRenderPoints( std::vector<cv::Point2i> points , int penWidth, cv::Vec3b penColor, QString description_key  , cv::Point2i margin_point, int alpha_channel_value ){
  std::vector<cv::Vec3b> penColors = std::vector<cv::Vec3b>();
  for( int pos = 0; pos< points.size(); pos++ ){
    penColors.push_back( penColor );
  }
  addRenderPoints( points ,  penWidth, penColors, description_key  , margin_point,  alpha_channel_value );
}

void CVImageWidget::addRenderPoints( std::vector<cv::Point2i> points , int penWidth, std::vector<cv::Vec3b> penColors, QString description_key, cv::Point2i margin_point, int alpha_channel_value ){

  std::map< QString,std::vector<QPoint> >::iterator it = renderPoints_map.find( description_key );
  if ( it != renderPoints_map.end() ){
    renderPoints_map.erase (it);
  }
  std::map< QString,std::vector<QColor> >::iterator it1 = renderPoints_penColor_map.find( description_key );
  if ( it1 != renderPoints_penColor_map.end() ){
    renderPoints_penColor_map.erase (it1);
  }
  std::map< QString,std::vector<int> >::iterator it2 = renderPoints_penWidth_map.find( description_key );
  if ( it2 != renderPoints_penWidth_map.end() ){
    renderPoints_penWidth_map.erase (it2);
  }
  std::map< QString,std::vector<QPoint> >::iterator it3 = renderPoints_margin_points_map.find( description_key );
  if ( it3 != renderPoints_margin_points_map.end() ){
    renderPoints_margin_points_map.erase (it3);
  }
  std::map< QString,std::vector<int> >::iterator it4 = renderPoints_alpha_channel_map.find( description_key );
  if ( it4 != renderPoints_alpha_channel_map.end() ){
    renderPoints_alpha_channel_map.erase (it4);
  }
  std::map< QString,std::vector<bool> >::iterator it5 = renderPoints_Selection_map.find( description_key );
  if ( it5 != renderPoints_Selection_map.end() ){
    renderPoints_Selection_map.erase (it5);
  }

  std::vector<QPoint> renderPoints = std::vector<QPoint>();
  std::vector<bool>renderPoints_penSelection = std::vector<bool>();
  std::vector<QColor>renderPoints_penColors = std::vector<QColor>();
  std::vector<int> renderPoints_penWidth = std::vector<int>();
  std::vector<QPoint> renderPoints_margin_points = std::vector<QPoint>();
  std::vector<int> renderPoints_alpha_channel = std::vector<int>();

  for( int pos = 0; pos< points.size(); pos++ ){
    const cv::Vec3b penColor = penColors[pos];
    const QColor point_penColor = QColor(penColor[0], penColor[1], penColor[2] );
    //std::cout << "\taddRenderPoints point # " << pos << " " << penColor << " " << point_penColor.name().toStdString() << std::endl;

    renderPoints_penColors.push_back(point_penColor);
    renderPoints_penSelection.push_back( false );
    renderPoints_penWidth.push_back( penWidth );
    cv::Point2i pt = points[pos];
    renderPoints.push_back( QPoint (pt.x, pt.y) );
    renderPoints_margin_points.push_back( QPoint( margin_point.x, margin_point.y ) );
    renderPoints_alpha_channel.push_back( alpha_channel_value );
  }

  renderPoints_map.insert ( std::pair< QString,std::vector<QPoint> >( description_key , renderPoints ) );
  renderPoints_Selection_map.insert ( std::pair< QString,std::vector<bool> >( description_key , renderPoints_penSelection ) );
  renderPoints_penColor_map.insert ( std::pair< QString,std::vector<QColor> >( description_key , renderPoints_penColors ) );
  renderPoints_penWidth_map.insert ( std::pair< QString,std::vector<int> >( description_key , renderPoints_penWidth ) );
  renderPoints_margin_points_map.insert ( std::pair< QString,std::vector<QPoint> >( description_key , renderPoints_margin_points ) );
  renderPoints_alpha_channel_map.insert ( std::pair< QString,std::vector<int> >( description_key , renderPoints_alpha_channel ) );
}

void CVImageWidget::addRenderPoints_key_to_group( QString key_string, QString group_key ){

  std::map< QString,std::vector<QString> >::iterator it4 = renderPoints_group_keys_map.find( group_key );
  if ( it4 == renderPoints_group_keys_map.end() ){
    std::vector<QString> renderPoints_group = std::vector<QString>();
    renderPoints_group.push_back( key_string );
    renderPoints_group_keys_map.insert ( std::pair< QString,std::vector<QString> >( group_key , renderPoints_group ) );
  }
  else{
    std::vector<QString> renderPoints_group = it4->second;
    renderPoints_group.push_back( key_string );
    it4->second = renderPoints_group;
  }

}

bool CVImageWidget::set_renderPoints_group_alpha_channels_map( QString group_key, int value ){
  bool result = false;
  std::map< QString,std::vector<QString> >::iterator it4 = renderPoints_group_keys_map.find( group_key );
  if ( it4 != renderPoints_group_keys_map.end() ){
   std::vector<QString> renderPoints_group = it4->second;
   result = true;
   for( QString key : renderPoints_group ){
    result &= set_renderPoints_alpha_channels_map( key, value );
  }
  //std::cout << "set_renderPoints_group_alpha_channels_map ( " << value << " ) . group size: " << renderPoints_group.size() << " final result " << std::boolalpha << result << std::endl;
}
return result;
}

void CVImageWidget::cleanRenderAreas(){
  renderAreas.clear();
  renderAreas_penWidth.clear();
  renderAreas_penColor.clear();
  renderAreas_top_left.clear();
}

int CVImageWidget::addImageLayer( const cv::Mat& image ){
  alpha_channels.push_back( 255 );
  return -1;
} 

int CVImageWidget::get_image_layer_alpha_channel( int layer_number ){
  int result = 255;
  if( layer_number >= 0 && layer_number < alpha_channels.size() ){
    result = alpha_channels[layer_number];
  }
  return result;
}

bool CVImageWidget::set_image_layer_alpha_channel( int layer_number, int value ){
  bool result = false;
  if( layer_number >= 0 && layer_number < alpha_channels.size() ){
    alpha_channels[layer_number] = value;
    result = true;
  }
  return result;
}

int CVImageWidget::get_renderPoints_alpha_channels_map( QString description_key ) {
  int result = 255;
  std::map< QString,std::vector<int> >::iterator it_alpha = renderPoints_alpha_channel_map.find( description_key );
  if ( it_alpha != renderPoints_alpha_channel_map.end() ){
    std::vector<int> vec = it_alpha->second;
    result = vec[0];
  }
  return result;
}

bool CVImageWidget::set_renderPoints_alpha_channels_map( QString description_key, int value ) {
  bool result = false;
  std::map< QString,std::vector<int> >::iterator it_alpha = renderPoints_alpha_channel_map.find( description_key );
  if ( it_alpha != renderPoints_alpha_channel_map.end() ){
    for( std::vector<int>::iterator it = it_alpha->second.begin(); it != it_alpha->second.end(); ++it ){
      *it = value;
    }
    result = true;
  }
  return result;
}

bool CVImageWidget::set_render_point_selected_state( QString key_string, bool value ){
  bool result = false;
  std::map< QString,std::vector<bool> >::iterator it_selected = renderPoints_Selection_map.find( key_string );
  if ( it_selected != renderPoints_Selection_map.end() ){
    for( std::vector<bool>::iterator it = it_selected->second.begin(); it != it_selected->second.end(); ++it ){
      *it = value;
    }
    result = true;
  }
  return result;
}

void CVImageWidget::paintEvent(QPaintEvent* event) {
  // Display the image
  QPainter painter(this);

  for( int pos = 0; pos < images.size(); pos++ ){
    if( images_set[pos] ){
      cv::Point2i margin_point = margin_points[pos];
      painter.save();
      double dopacity = alpha_channels[pos] / 255.0f;
      painter.setOpacity( dopacity ); 
      painter.drawImage( QPoint(margin_point.x * scaleFactor ,margin_point.y * scaleFactor), images[pos] );
      painter.restore();
    }
  }
  
  // Draw the paths
  for( int list_position = 0; list_position < renderAreas.size() ; list_position++ ){
    const bool _area_visible = renderAreas_visible.at( list_position );
    if( _area_visible ){
      painter.save();
      cv::Point2i _top_left = renderAreas_top_left.at( list_position );
      _top_left *= scaleFactor;
      QPainterPath _area = renderAreas.at(list_position);
      const int _area_pen_width = renderAreas_penWidth.at(list_position);
      QColor _area_pen_color = renderAreas_penColor.at(list_position);
      painter.translate( _top_left.x, _top_left.y );
      painter.setPen(QPen(_area_pen_color, _area_pen_width, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
      painter.scale( scaleFactor, scaleFactor );
      painter.drawPath(_area);
      painter.restore();
    }
  }

  // Draw selection rectangle
  if( _started_rectangleSelection ){
    painter.save();
    painter.setPen(QPen(QBrush(QColor(0,0,0,180)),1,Qt::DashLine));
    painter.setBrush(QBrush(QColor(255,255,255,120)));
    painter.drawRect(selectionRect);
    painter.restore();

  }

  // Draw statistical selection rectangle
  if( _started_rectangleSelectionStatistical ){
    painter.save();
    painter.setPen(QPen(QBrush(QColor(0,0,0,180)),1,Qt::DashLine));
    painter.setBrush(QBrush(QColor(255,255,255,120)));
    painter.drawRect(selectionStatisticalRect);
    painter.restore();
  }

  

  for ( std::map< QString,std::vector<QPoint>>::iterator it=renderPoints_map.begin(); it!=renderPoints_map.end(); ++it){
    const QString key = it->first;

    std::map< QString,std::vector<QColor> >::iterator it_colors = renderPoints_penColor_map.find( key );
    std::map< QString,std::vector<bool> >::iterator it_selected = renderPoints_Selection_map.find( key );
    std::map< QString,std::vector<int> >::iterator it_width = renderPoints_penWidth_map.find( key );
    std::map< QString,std::vector<QPoint> >::iterator it_margin = renderPoints_margin_points_map.find( key );
    std::map< QString,std::vector<int> >::iterator it_alpha = renderPoints_alpha_channel_map.find( key );
    
    if (  it_colors != renderPoints_penColor_map.end() &&  
      it_selected != renderPoints_Selection_map.end() &&  
      it_width != renderPoints_penWidth_map.end() &&
      it_margin != renderPoints_margin_points_map.end() && 
      it_alpha != renderPoints_alpha_channel_map.end() 
      ){
      const std::vector<QPoint> renderPoints = it->second;
    const std::vector<QColor> renderPoints_penColor = it_colors->second;
    const QString key_color = it_colors->first;

    const std::vector<int> renderPoints_penWidth = it_width->second;
    const std::vector<QPoint> renderPoints_margin_points = it_margin->second;
    const std::vector<int> render_points_alpha_channel = it_alpha->second;
    const std::vector<bool> render_points_selection = it_selected->second;
/*
    std::cout << "#### renderPoints.size() " << renderPoints.size() << std::endl;
    std::cout << "#### renderPoints_Selection_map.size() " << renderPoints_Selection_map.size() << std::endl;
    std::cout << "#### renderPoints_penColor.size() " << renderPoints_penColor.size() << " key: " << key_color.toStdString() << std::endl;
    std::cout << "#### renderPoints_penWidth.size() " << renderPoints_penWidth.size() << std::endl;
    std::cout << "#### renderPoints_margin_points.size() " << renderPoints_margin_points.size() << std::endl;
    std::cout << "#### render_points_alpha_channel.size() " << render_points_alpha_channel.size() << std::endl;
*/
    for( int point_n = 0; point_n < renderPoints.size(); point_n++ ){
      const QPoint point_w_margin = renderPoints_margin_points[point_n] + renderPoints[point_n];
      const QColor point_penColor = renderPoints_penColor[point_n];
      const bool selected = render_points_selection[point_n];
      int point_penWidth = renderPoints_penWidth[point_n];
      point_penWidth *= selected ? 2 : 1;
      const double dopacity = render_points_alpha_channel[point_n] / 255.0f;
      //std::cout << " point # " << point_n << " x: " << point_w_margin.x() << " y: " << point_w_margin.y() <<  " " << point_penColor.name().toStdString() << "\t point_penWidth " << point_penWidth << "\t dopacity "<<  dopacity << std::endl;
      painter.save();
      painter.setOpacity( dopacity ); 
      painter.setPen(QPen(QBrush( point_penColor ),point_penWidth,Qt::DashLine));
      painter.setBrush(QBrush( point_penColor ));
      painter.scale( scaleFactor, scaleFactor );
      painter.drawPoint( point_w_margin );
      painter.restore();
    }
  }
}


painter.end();
}

void CVImageWidget::startRectangleSelection(){
  _enabled_rectangleSelection = true;
  setCursor( Qt::CrossCursor );
}


void CVImageWidget::startStatisticalRectangleSelection(){
  _enabled_rectangleSelectionStatistical = true;
  setCursor( Qt::CrossCursor );
}

void CVImageWidget::mousePressEvent(QMouseEvent *e){
  if ( e->button()==Qt::LeftButton ){
    if( _enabled_rectangleSelection ){
      _started_rectangleSelection = true;
      selectionRect.setTopLeft(e->pos());
      selectionRect.setBottomRight(e->pos());
    }
    if( _enabled_rectangleSelectionStatistical ){
      _started_rectangleSelectionStatistical = true;
      selectionStatisticalRect.setTopLeft(e->pos());
      selectionStatisticalRect.setBottomRight(e->pos());
    }
    if( _enable_map_pos_signal ){
      emit MapPosClicked( e->pos() );
    }
  }
}

void CVImageWidget::mouseMoveEvent(QMouseEvent *e){
  if ( _started_rectangleSelection ){
    selectionRect.setBottomRight(e->pos());
    repaint();
  }
  if ( _started_rectangleSelectionStatistical ){
    selectionStatisticalRect.setBottomRight(e->pos());
    repaint();
  }
}

QRect CVImageWidget::mapSelectionRectToOriginalSize(){
  QRect original_size_selectionRect;
  original_size_selectionRect.setX( selectionRect.x() / scaleFactor );
  original_size_selectionRect.setY( selectionRect.y() / scaleFactor );
  original_size_selectionRect.setWidth( selectionRect.width() / scaleFactor );
  original_size_selectionRect.setHeight( selectionRect.height() / scaleFactor );
  return original_size_selectionRect;
}

QRect CVImageWidget::mapSelectionStatisticalRectToOriginalSize(){
  QRect original_size_selectionRect;
  original_size_selectionRect.setX( selectionStatisticalRect.x() / scaleFactor );
  original_size_selectionRect.setY( selectionStatisticalRect.y() / scaleFactor );
  original_size_selectionRect.setWidth( selectionStatisticalRect.width() / scaleFactor );
  original_size_selectionRect.setHeight( selectionStatisticalRect.height() / scaleFactor );
  return original_size_selectionRect;
}

void CVImageWidget::mouseReleaseEvent(QMouseEvent *e){
  cv::Size current_size(0,0);
  if( current_sizes.size() > 0 ){
    current_size = current_sizes[0];
  }

  if( _started_rectangleSelection ){
    _started_rectangleSelection = false;
    _enabled_rectangleSelection = false;
    setCursor( Qt::ArrowCursor );

    //make sure the selection rectangle does not surpases the original image size
    if( selectionRect.bottom() > current_size.height ){
      selectionRect.setBottom( current_size.height );
    }
    if( selectionRect.right() > current_size.width ){
      selectionRect.setRight( current_size.width );
    }
    const QRect original_size_selectionRect = mapSelectionRectToOriginalSize();
    emit selectionRectangleChanged( original_size_selectionRect );
  }

  if( _started_rectangleSelectionStatistical ){
    _started_rectangleSelectionStatistical = false;
    _enabled_rectangleSelectionStatistical = false;
    setCursor( Qt::ArrowCursor );

    //make sure the selection rectangle does not surpases the original image size
    if( selectionStatisticalRect.bottom() > current_size.height ){
      selectionStatisticalRect.setBottom( current_size.height );
    }
    if( selectionStatisticalRect.right() > current_size.width ){
      selectionStatisticalRect.setRight( current_size.width );
    }
    const QRect original_size_selectionStatisticalRect = mapSelectionStatisticalRectToOriginalSize();
    emit selectionStatisticalRectangleChanged( original_size_selectionStatisticalRect );
  }

  repaint();
}
