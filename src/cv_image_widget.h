#pragma once
#include <QWidget>
#include <QtWidgets>
#include <QAbstractItemView>
#include <QImage>
#include <QPainter>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <QtGui>
#include <QColor>
#include <QHash>
#include <QWidget>

#include "renderarea.h"

class CVImageWidget : public QWidget
{
  Q_OBJECT
  public:
    explicit CVImageWidget(QWidget *parent = 0);

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    void set_container_window_size( const int width , const int height );

    public slots:

      /* right click context menu */
      void ShowContextMenu(const QPoint &pos);

    /*image controls*/
    void zoomIn();
    void zoomOut();
    void normalSize();
    void fitToWindow();

    /*image setters*/
    void setImage(const cv::Mat& image);
    void updateImage();

    /*shape setters*/
    void addRect( cv::Rect _area_rect, int penWidth , QString description );
    void addRect( cv::Rect _area_rect, int penWidth, cv::Vec3b penColor , QString description );
    void addShapePolygon( std::vector<cv::Point2i> polygon , cv::Point2i top_left,  int penWidth, QString description );
    void addShapePolygon( std::vector<cv::Point2i> polygon , cv::Point2i top_left,  int penWidth, cv::Vec3b penColor, QString description );
    void cleanRenderAreas();

signals:

  protected:
    void paintEvent(QPaintEvent* event);

  private:

    QList<QPainterPath> renderAreas;
    QList<int> renderAreas_penWidth;
    QList<QString> renderAreas_description;
    QList<bool> renderAreas_visible;
    QList<QColor> renderAreas_penColor;
    std::vector<cv::Point2i> renderAreas_top_left;
    cv::Vec3b renderAreas_default_color = cv::Vec3b(255,0,0);
    
    QImage _qimage;
    cv::Mat _tmp_original, _tmp_current;
    float scaleFactor = 1.0f;
    cv::Size original_size;
    bool _image_set = false;
    int _container_window_width = 0;
    int _container_window_height = 0;
};
