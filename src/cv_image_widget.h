#pragma once
#include <QWidget>
#include <QtWidgets>
#include <QAbstractItemView>
#include <QImage>
#include <QPainter>
#include <opencv2/opencv.hpp>
#include <iostream>

class CVImageWidget : public QWidget
{
  Q_OBJECT
  public:
    explicit CVImageWidget(QWidget *parent = 0) : QWidget(parent) , scaleFactor(1) { }

    QSize sizeHint() const { return _qimage.size(); }
    QSize minimumSizeHint() const { return _qimage.size(); }

    void set_container_window_size( const int width , const int height ){
      _container_window_width = width;
      _container_window_height = height;
    }

    public slots:

      void zoomIn(){
        scaleFactor += 0.10;
        updateImage();
      }
    void zoomOut(){
      scaleFactor -= 0.10;
      if(scaleFactor < 0.10 ){scaleFactor = 0.10;}
      updateImage();
    }
    void normalSize(){
      scaleFactor = 1;
      updateImage();
    }

    void fitToWindow( ){
      int window_width = _container_window_width;
      int window_height = _container_window_height;
      float w_factor = ((float) window_width) / ((float) original_size.width );
      float h_factor = ((float) window_height) / ((float) original_size.height );
      scaleFactor = std::min(w_factor,h_factor);
      updateImage();
    }

    void setImage(const cv::Mat& image) {
      // Convert the image to the RGB888 format
      switch (image.type()) {
        case CV_8UC1:
          cvtColor(image, _tmp_original, CV_GRAY2RGB);
          break;
        case CV_8UC3:
          cvtColor(image, _tmp_original, CV_BGR2RGB);
          break;
      }

      // QImage needs the data to be stored continuously in memory
      assert( _tmp_original.isContinuous() );
      original_size = _tmp_original.size();
      // Assign OpenCV's image buffer to the QImage. Note that the bytesPerLine parameter
      // (http://qt-project.org/doc/qt-4.8/qimage.html#QImage-6) is 3*width because each pixel
      // has three bytes.
      _qimage = QImage(_tmp_original.data, _tmp_original.cols, _tmp_original.rows, _tmp_original.cols*3, QImage::Format_RGB888);
      this->setFixedSize(image.cols, image.rows);
      _image_set = true;
      repaint();
    }

    void updateImage() {
      if( _image_set ){
        // Convert the image to the RGB888 format
        cv::Size newsize(original_size.width * scaleFactor , original_size.height * scaleFactor);//the _tmp_current image size,e.g.100x100
        cv::resize(_tmp_original,_tmp_current,newsize);//resize image

        // QImage needs the data to be stored continuously in memory
        assert(_tmp_current.isContinuous());
        // Assign OpenCV's image buffer to the QImage. Note that the bytesPerLine parameter
        // (http://qt-project.org/doc/qt-4.8/qimage.html#QImage-6) is 3*width because each pixel
        // has three bytes.
        _qimage = QImage(_tmp_current.data, _tmp_current.cols, _tmp_current.rows, _tmp_current.cols*3, QImage::Format_RGB888);
        this->setFixedSize(_tmp_current.cols, _tmp_current.rows);
        repaint();
      }
    }
  protected:
    void paintEvent(QPaintEvent* /*event*/) {
      // Display the image
      QPainter painter(this);
      painter.drawImage(QPoint(0,0), _qimage);
      painter.end();
    }

    QImage _qimage;
    cv::Mat _tmp_original, _tmp_current;
    float scaleFactor = 1.0f;
    cv::Size original_size;
    bool _image_set = false;
    int _container_window_width = 0;
    int _container_window_height = 0;
};


