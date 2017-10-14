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

class CvImageFrameWidget : public QWidget
{
  Q_OBJECT
  public:
    explicit CvImageFrameWidget(QWidget *parent = 0);

    public slots:

      /* image widget */
      void setImage(const cv::Mat& image);
    void setImageWidget( CVImageWidget* widget );

    /* shapes in image widget */
    void cleanRenderAreas();
    void addShapeRect( cv::Rect _rectangle, int pen_width, QString shape_description );
    void addShapeRect( cv::Rect _rectangle, int pen_width, cv::Vec3b pen_color, QString shape_description );
    void addShapePolygon( std::vector<cv::Point2i> polygon , cv::Point2i top_left,  int pen_width, QString shape_description );
    void addShapePolygon( std::vector<cv::Point2i> polygon , cv::Point2i top_left,  int pen_width, cv::Vec3b pen_color, QString shape_description );
    void enableRectangleSelection();
    void emit_selectionRectangleChanged( QRect );
    void emit_selectionStatisticalRectangleChanged( QRect );

    signals:
      void selectionRectangleChanged(QRect);
      void selectionStatisticalRectangleChanged(QRect);

  protected:
    CVImageWidget *image_widget;
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
};

#endif
