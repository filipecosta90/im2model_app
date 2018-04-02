/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#include "cv_tdmap_cell_image_frame_delegate.h"

void CvTDMapImageFrameDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
  // save the painter's state
  painter->save();

  painter->setRenderHint(QPainter::Antialiasing);

  if ( option.state & QStyle::State_Selected ){
    painter->setPen(QPen(QColor(255,0,0),1));
    painter->setBrush(QBrush(QColor(255, 0, 0)));
  }

  if ( shouldBeBest(index) ){
    painter->setPen(QPen(QColor(0,255,0),1));
    painter->setBrush(QBrush(QColor(0, 255, 0)));
  }

  QStyleOptionViewItem itemOption(option);
  painter->drawRect(itemOption.rect );

  // restore the painter's state
  painter->restore();
}

/* Loggers */
bool CvTDMapImageFrameDelegate::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
  BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification, "Application logger setted for CvTDMapImageFrameDelegate class." );
  return true;
}

bool CvTDMapImageFrameDelegate::shouldBeBest(const QModelIndex &index) const {
  bool result = false;
  if( (index.row() == _best_row) && (index.column() == _best_col) && (_best_defined) ){
    result = true;
  }
  return result;
}

void CvTDMapImageFrameDelegate::set_best( int row, int col ){
  _best_row = row;
  _best_col = col;
  _best_defined = true;
}

void CvTDMapImageFrameDelegate::clean_best( ){
  _best_defined = false;
}
