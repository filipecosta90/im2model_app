/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#ifndef SRC_CVTDMAPCELLIMAGEFRAMEDELEGATE_H__
#define SRC_CVTDMAPCELLIMAGEFRAMEDELEGATE_H__

#include <QWidget>
#include <QtWidgets>
#include <QAbstractItemView>
#include <QImage>
#include <QPainter>
#include <opencv2/opencv.hpp>
#include <QFrame>
#include <QBoxLayout>
#include <QAbstractItemDelegate>
#include <QModelIndex>
#include <QSize>

#include <QStyledItemDelegate>

#include "cv_tdmap_cell_image_frame.h"
#include "cv_image_frame.h"
#include "cv_image_cell_widget.h"

#include "base_logger.hpp"

class QAbstractItemModel;
class QObject;
class QPainter;

class CvTDMapImageFrameDelegate : public QStyledItemDelegate {
  Q_OBJECT
  public:
    CvTDMapImageFrameDelegate(int margin, QWidget *parent = 0) : QStyledItemDelegate(parent),  m_margin(margin) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool shouldBeBest(const QModelIndex &index) const;
    bool shouldBeSelected(const QModelIndex &index) const;
    void set_best( int row, int col );
    void set_selected( int row, int col );
    void clean_best( );
    void clean_selected( );
  bool set_application_logger( ApplicationLog::ApplicationLog* logger );

  private:
    // margin between cells
    int m_margin;
    int _best_row;
    int _best_col;
    bool _best_defined = false;
    int _selected_row;
    int _selected_col;
    bool _selected_defined = false;

        /* Loggers */
    ApplicationLog::ApplicationLog* logger = nullptr;
    bool _flag_logger = false;

};

#endif // SRC_CVTDMAPCELLIMAGEFRAMEDELEGATE_H__
