#ifndef SRC_CVIMAGEWIDGET_H__
#define SRC_CVIMAGEWIDGET_H__

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
    QSize largestImageLayerSize() const;
    void set_container_window_size( const int width , const int height );

    public slots:

    void startRectangleSelection();
    void startStatisticalRectangleSelection();

    /* right click context menu */
    void ShowContextMenu(const QPoint &pos);

    /*image controls*/
    void zoomIn();
    void zoomOut();
    void normalSize();
    void fitToWindow();

    /*image setters*/
    void setImage(const cv::Mat& image, int layer_number = 0, QString ImageDescription = "", cv::Point2i margin_point = cv::Point2i(0,0) );
    int addImageLayer( const cv::Mat& image );
    void updateImage();
    int get_image_layer_alpha_channel( int layer_number );
    bool set_image_layer_alpha_channel( int layer_number, int value );

    /*shape setters*/
    void addRect( cv::Rect _area_rect, int penWidth , QString description );
    void addRect( cv::Rect _area_rect, int penWidth, cv::Vec3b penColor , QString description );
    void addShapePolygon( std::vector<cv::Point2i> polygon , cv::Point2i top_left,  int penWidth, QString description );
    void addShapePolygon( std::vector<cv::Point2i> polygon , cv::Point2i top_left,  int penWidth, cv::Vec3b penColor, QString description );
    void addRenderPoints( std::vector<cv::Point2i> points , int penWidth, cv::Vec3b penColor, QString description, cv::Point2i margin_point = cv::Point2i(0,0) );
    void cleanRenderAreas();

    signals:
    void selectionRectangleChanged( QRect );
    void selectionStatisticalRectangleChanged( QRect );

protected:
    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private:

    QRect mapSelectionRectToOriginalSize();
    QRect mapSelectionStatisticalRectToOriginalSize();

    QList<QPainterPath> renderAreas;
    QList<int> renderAreas_penWidth;
    QList<QString> renderAreas_description;
    QList<bool> renderAreas_visible;
    QList<QColor> renderAreas_penColor;
    std::vector<cv::Point2i> renderAreas_top_left;
    cv::Vec3b renderAreas_default_color = cv::Vec3b(255,0,0);
    std::vector<QPoint> renderPoints;

    std::vector<int> alpha_channels;
    std::vector<QImage> images;
    std::vector<cv::Mat> _tmp_originals;
    std::vector<cv::Mat> _tmp_currents;
    std::vector<cv::Size> original_sizes;
    std::vector<cv::Size> current_sizes;
    std::vector<bool> images_set;
    std::vector<cv::Point2i> margin_points;
    std::vector<cv::Point2i> renderPoints_margin_points;


    float scaleFactor = 1.0f;
    int _container_window_width = 0;
    int _container_window_height = 0;

    bool _enabled_rectangleSelection = false;
    bool _started_rectangleSelection = false;
    bool freeSelection = false;
    bool pathSelection = false;
    QRect selectionRect;

    QRect selectionStatisticalRect;
    bool _enabled_rectangleSelectionStatistical = false;
    bool _started_rectangleSelectionStatistical = false;

};

#endif
