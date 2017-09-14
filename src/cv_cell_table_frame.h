#ifndef SRC_CVCELLTABLEFRAME_H__
#define SRC_CVCELLTABLEFRAME_H__

#include <QWidget>
#include <QtWidgets>
#include <QAbstractItemView>
#include <QImage>
#include <QPainter>
#include <opencv2/opencv.hpp>
#include <QFrame>
#include <QBoxLayout>
#include <QTableView>

#include "vis/unit_cell_view_widget.h"

class CvCellFrameTableWidget : public UnitCellViewerWindow
{
  Q_OBJECT
  public:
    explicit CvCellFrameTableWidget(QWidget *parent = 0) : UnitCellViewerWindow(parent){
    table_view = new QTableView;
    //disable editing
    table_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_view->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    table_view->resizeRowsToContents();
    table_view->setMaximumHeight(0);
    m_containerLayout->addWidget(table_view);
  }
    public slots:

      void setModel(QAbstractItemModel* model ){
        table_view->setModel(model);
        table_view->resizeRowsToContents();
        int row_heigth = table_view->rowHeight(0) * model->rowCount();
        int header_heigth = table_view->horizontalHeader()->height();
        table_view->setMaximumHeight(row_heigth + header_heigth);
        m_containerLayout->update();
        table_view->show();
      }

  protected:
    QTableView *table_view;

};

#endif
