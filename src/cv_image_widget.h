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
#include <map>


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
    void MapPosToObject( const QPoint &pos );

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
    bool set_render_point_selected_state( QString key_string, bool value );
    bool set_renderPoints_alpha_channels_map( QString key_string, int value );
    int get_renderPoints_alpha_channels_map( QString description_key );

    /*shape setters*/
    void addRect( cv::Rect _area_rect, int penWidth , QString description );
    void addRect( cv::Rect _area_rect, int penWidth, cv::Vec3b penColor , QString description );
    void addShapePolygon( std::vector<cv::Point2i> polygon , cv::Point2i top_left,  int penWidth, QString description );
    void addShapePolygon( std::vector<cv::Point2i> polygon , cv::Point2i top_left,  int penWidth, cv::Vec3b penColor, QString description );
    void addRenderPoints( std::vector<cv::Point2i> points , int penWidth, std::vector<cv::Vec3b> penColor, QString key_string, cv::Point2i margin_point = cv::Point2i(0,0), int alpha_channel_value = 255 );
    void addRenderPoints( std::vector<cv::Point2i> points , int penWidth, cv::Vec3b penColor, QString key_string,  cv::Point2i margin_point = cv::Point2i(0,0), int alpha_channel_value = 255 );
    void cleanRenderAreas();

    signals:
    void selectionRectangleChanged( QRect );
    void selectionStatisticalRectangleChanged( QRect );
    void MapPosClicked( QPoint );

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

    std::map< QString,std::vector<QPoint> > renderPoints_map;
    std::map< QString,std::vector<bool> > renderPoints_Selection_map;
    std::map< QString,std::vector<QColor> > renderPoints_penColor_map;
    std::map< QString,std::vector<int> > renderPoints_penWidth_map;
    std::map< QString,std::vector<QPoint> > renderPoints_margin_points_map;
    std::map< QString,std::vector<int> > renderPoints_alpha_channel_map;

    std::vector<int> alpha_channels;
    std::vector<QImage> images;
    std::vector<cv::Mat> _tmp_originals;
    std::vector<cv::Mat> _tmp_currents;
    std::vector<cv::Size> original_sizes;
    std::vector<cv::Size> current_sizes;
    std::vector<bool> images_set;
    std::vector<cv::Point2i> margin_points;

    float scaleFactor = 1.0f;
    int _container_window_width = 0;
    int _container_window_height = 0;

    bool _enabled_rectangleSelection = false;
    bool _started_rectangleSelection = false;
    bool _enable_map_pos_signal = true;
    bool freeSelection = false;
    bool pathSelection = false;
    QRect selectionRect;

    QRect selectionStatisticalRect;
    bool _enabled_rectangleSelectionStatistical = false;
    bool _started_rectangleSelectionStatistical = false;

};

#endif
