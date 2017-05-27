
#ifndef CVTDMAPIMAGEFRAME_H
#define CVTDMAPIMAGEFRAME_H

#include <QWidget>
#include <QtWidgets>
#include <QAbstractItemView>
#include <QImage>
#include <QPainter>
#include <opencv2/opencv.hpp>
#include <QFrame>
#include <QBoxLayout>
#include <QMetaType>

#include "cv_image_widget.h"

class CvTDMapImageFrame {

  public:
    explicit CvTDMapImageFrame();
    CvTDMapImageFrame( QWidget * parent );

    void setImage(const cv::Mat& image);
  void setImageWidget( CVImageWidget* widget );
  bool _is_image_setted();

  protected:
    CVImageWidget *image_widget;
    QWidget *parentWidget;
    QScrollArea* scrollArea;
    QHBoxLayout *contentsLayout;
    QBoxLayout *toolsLayout;
    QToolBar *toolbar;
    bool  _flag_image_setted = false;

};

Q_DECLARE_METATYPE(CvTDMapImageFrame)

#endif
