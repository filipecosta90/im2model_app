#include "texture_image.h"

void TextureImage::paint (QPainter* painter)
{
/*  int w = painter->device()->width();
  int h = painter->device()->height();
  /* Clear to white. */
  //painter->fillRect(0, 0, w, h, QColor(255, 255, 255));

  if( _image_set ){
    bool b = _qimage_ptr->save("test2.png");
    painter->drawImage( QPoint(0,0), *_qimage_ptr );
    std::cout << " paiting image " << std::endl;
    bool c = _qimage_ptr->save("test3.png");
    std::cout << " save result 2 " << std::boolalpha << b << std::endl;
  }
}

void TextureImage::setImage( const cv::Mat& image ){
  // Convert the image to the RGB888 format
  switch (image.type()) {
    case cv::DataType<unsigned char>::type:
    std::cout << "$$$ cv::DataType<unsigned char>::type " << std::endl;
    cvtColor(image, _tmp_original, CV_GRAY2RGB);
    cv::imwrite( "_tmp_original_char.png", _tmp_original );
    cv::imwrite( "image.png", image );
    break;
    case cv::DataType<unsigned short>::type:
    {
      cv::Mat temp;
      std::cout << "$$$ cv::DataType<unsigned short>::type " << std::endl;
      image.convertTo(temp, cv::DataType<unsigned char>::type, 1.0f/255.0f );
      cvtColor(temp, _tmp_original, CV_GRAY2RGB );
      cv::imwrite( "_tmp_original_short.png", _tmp_original );
      cv::imwrite( "temp_short.png", temp );
      break;
    }
    case CV_8UC3:
    cvtColor(image, _tmp_original, CV_BGR2RGB);
    break;
  }

  // QImage needs the data to be stored continuously in memory
  if ( ! _tmp_original.isContinuous() )
  { 
    _tmp_original = _tmp_original.clone();
  }
  //assert( _tmp_original.isContinuous() );
  original_size = image.size();


  std::cout << "TextureImage::setImage from image with original size " << original_size << std::endl;

  
  cv::split(_tmp_original, matChannels);
  
  // create alpha channel
  cv::Mat alpha ( _tmp_original.cols, _tmp_original.rows, CV_8UC1, cv::Scalar(255) );
 //matChannels.at(0) + matChannels.at(1) + matChannels.at(2);
  matChannels.push_back(alpha);
  
  cv::merge(matChannels, dst);
  // QImage needs the data to be stored continuously in memory
  if ( ! dst.isContinuous() )
  { 
    dst = dst.clone();
  }
  cv::imwrite( "dst.png", dst );

  // Assign OpenCV's image buffer to the QImage. Note that the bytesPerLine parameter
  // (http://qt-project.org/doc/qt-4.8/qimage.html#QImage-6) is 4*width because each pixel
  // has three bytes.
  // QImage(uchar *data, int width, int height, int bytesPerLine, Format format, QImageCleanupFunction cleanupFunction = Q_NULLPTR, void *cleanupInfo = Q_NULLPTR)
  _qimage_ptr = new QImage(dst.data, dst.cols, dst.rows, dst.cols*4, QImage::Format_RGBA8888);
  this->setSize( QSize( image.cols, image.rows ) );
  bool b = _qimage_ptr->save("test.png");
  std::cout << " save result " << std::boolalpha << b << std::endl;
  _image_set = true;
}
