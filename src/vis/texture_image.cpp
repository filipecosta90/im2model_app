#include "texture_image.h"

void TextureImage::paint (QPainter* painter)
{
/*  int w = painter->device()->width();
  int h = painter->device()->height();
  /* Clear to white. */
  //painter->fillRect(0, 0, w, h, QColor(255, 255, 255));

  if( _image_set ){
    painter->drawImage( QPoint(0,0), *_qimage_ptr );
  }
}

void TextureImage::setImage( const cv::Mat& image ){
  // Convert the image to the RGB888 format
  cv::Mat _tmp_original, _tmp_current;
  cv::Size original_size, current_size;

  switch (image.type()) {
    case cv::DataType<float>::type:
    {
      cv::Mat temp;
      image.convertTo(temp, cv::DataType<unsigned char>::type);
      cvtColor(temp, _tmp_original, CV_GRAY2RGB );
      break;
    }
    case cv::DataType<int>::type:
    case cv::DataType<unsigned int>::type:
    {
      cv::Mat temp;
      image.convertTo(temp, cv::DataType<unsigned char>::type, 1.0f/65536.0f);
      cvtColor(temp, _tmp_original, CV_GRAY2RGB );
      break;
    }
    case cv::DataType<char>::type:
    case cv::DataType<unsigned char>::type:
    {
      cvtColor(image, _tmp_original, CV_GRAY2RGB );
      break;
    }
    case cv::DataType<short>::type:
    case cv::DataType<unsigned short>::type:
    {
      cv::Mat temp;
      image.convertTo(temp, cv::DataType<unsigned char>::type, 1.0f/255.0f);
      cvtColor(temp, _tmp_original, CV_GRAY2RGB );
      break;
    }
    case CV_8UC3:
    cvtColor(image, _tmp_original, CV_BGR2RGB );
    break;
  }

  original_size = image.size();
  current_size = original_size;
  // Assign OpenCV's image buffer to the QImage. Note that the bytesPerLine parameter
  // (http://qt-project.org/doc/qt-4.8/qimage.html#QImage-6) is 3*width because each pixel
  // has three bytes.
  // QImage needs the data to be stored continuously in memory
  if ( ! _tmp_original.isContinuous() )
  { 
    _tmp_original = _tmp_original.clone();
  }

  _qimage_ptr = new QImage(_tmp_original.data, _tmp_original.cols, _tmp_original.rows, _tmp_original.cols*3, QImage::Format_RGB888);
  this->setSize( QSize( image.cols, image.rows ) );
  _image_set = true;
}
