/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

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
  explicit CvCellFrameTableWidget(QWidget *parent = 0) : UnitCellViewerWindow(parent, true){
    table_view = new QTableView;
    //disable editing
    table_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_view->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    table_view->resizeRowsToContents();
    table_view->setMaximumHeight(0);
    m_containerLayout->addWidget(table_view);
  }

    /* Loggers */
  bool set_application_logger( ApplicationLog::ApplicationLog* logger ){
    bool result = UnitCellViewerWindow::set_application_logger( logger );
    return result;
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
