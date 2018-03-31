/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#ifndef SRC_CVIMAGETABLEFRAME_H__
#define SRC_CVIMAGETABLEFRAME_H__

#include <QWidget>
#include <QtWidgets>
#include <QAbstractItemView>
#include <QImage>
#include <QPainter>
#include <opencv2/opencv.hpp>
#include <QFrame>
#include <QBoxLayout>
#include <QTableView>

#include "cv_image_widget.h"
#include "cv_image_frame.h"
#include "base_logger.hpp"

class CvImageFrameTableWidget : public CvImageFrameWidget
{
  Q_OBJECT
  public:
    explicit CvImageFrameTableWidget(QWidget *parent = 0) : CvImageFrameWidget(parent){
    table_view = new QTableView;
    //disable editing
    table_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_view->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    table_view->resizeRowsToContents();
    table_view->setMaximumHeight(0);
    contentsLayout->addWidget(table_view);
  }

   /* Loggers */
  bool set_application_logger( ApplicationLog::ApplicationLog* logger );
  
    public slots:

      void setModel(QAbstractItemModel* model ){
        table_view->setModel(model);
        table_view->resizeRowsToContents();
        int row_heigth = table_view->rowHeight(0) * model->rowCount();
        int header_heigth = table_view->horizontalHeader()->height();
        table_view->setMaximumHeight(row_heigth + header_heigth);
        contentsLayout->update();
        table_view->show();
      }

  protected:
    QTableView *table_view;

  private:
        /* Loggers */
    ApplicationLog::ApplicationLog* logger = nullptr;
    bool _flag_logger = false;


};

#endif
