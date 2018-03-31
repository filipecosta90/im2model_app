/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#ifndef SRC_CVIMAGEFRAME_H__
#define SRC_CVIMAGEFRAME_H__

#include <QWidget>
#include <QtWidgets>
#include <QAbstractItemView>
#include <QImage>
#include <QPainter>
#include <opencv2/opencv.hpp>
#include <QFrame>
#include <QBoxLayout>

#include "cv_image_widget.h"
#include "base_logger.hpp"

class CvImageFrameWidget : public QWidget
{
  Q_OBJECT
public:
    explicit CvImageFrameWidget(QWidget *parent = 0);
    CVImageWidget *image_widget;

     /* Loggers */
  bool set_application_logger( ApplicationLog::ApplicationLog* logger );

    public slots:

      /* image widget */
    //void setImage(const cv::Mat& image);
    void setImage(const cv::Mat& image, int layer_number = 0, QString ImageDescription = "", cv::Point2i margin_point = cv::Point2i(0,0) );

    void setImageWidget( CVImageWidget* widget );
    int addImageLayer( const cv::Mat& image );
    /* shapes in image widget */
    void cleanRenderAreas();
    void addShapeRect( cv::Rect _rectangle, int pen_width, QString shape_description );
    void addShapeRect( cv::Rect _rectangle, int pen_width, cv::Vec3b pen_color, QString shape_description );
    void addShapePolygon( std::vector<cv::Point2i> polygon , cv::Point2i top_left,  int pen_width, QString shape_description );
    void addShapePolygon( std::vector<cv::Point2i> polygon , cv::Point2i top_left,  int pen_width, cv::Vec3b pen_color, QString shape_description );
    void enableRectangleSelection();
    void emit_selectionRectangleChanged( QRect );
    void emit_selectionStatisticalRectangleChanged( QRect );
    void addRenderPoints( std::vector<cv::Point2i> points , int penWidth, std::vector<cv::Vec3b> penColor, QString description , cv::Point2i margin_point = cv::Point2i(0,0) );
    void addRenderPoints( std::vector<cv::Point2i> points , int penWidth, cv::Vec3b penColor, QString description , cv::Point2i margin_point = cv::Point2i(0,0) );
    int get_image_layer_alpha_channel( int layer_number );
    bool set_image_layer_alpha_channel( int layer_number, int value );
    bool set_render_point_selected_state( QString key_string, bool value );
    int get_renderPoints_alpha_channels_map( QString key );
    bool set_renderPoints_alpha_channels_map( QString key, int value );
    bool set_renderPoints_alpha_channels_map_group( QString group_key, int value );
    bool addRenderPoints_key_to_group( QString key_string, QString group_key );

    signals:
    void selectionRectangleChanged(QRect);
    void selectionStatisticalRectangleChanged(QRect);

protected:
    QWidget *parentWidget;
    QScrollArea* scrollArea;
    QVBoxLayout *contentsLayout;
    QBoxLayout *toolsLayout;
    QToolBar *toolbar;

    QAction *rectSelectAct;
    QAction *freeSelectAct;
    QAction *pathSelectAct;
    QAction *statistical_rectSelectAct;

    QMenu *toolsMenu;
    QMenu *selectionMenu;
    QMenu *statisticalMenu;
    QMenu *dataCollectionMenu;

    QToolButton* toolsButton;

private:
        /* Loggers */
    ApplicationLog::ApplicationLog* logger = nullptr;
    bool _flag_logger = false;

};

#endif
