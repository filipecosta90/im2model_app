/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

/* StackOverflow reference:
 * http://stackoverflow.com/questions/21390859/is-it-possible-for-qvalidator-to-show-a-popup
 */

#include <QDebug>
#include "qline_edit_tooltip.h"


QLineEditToolTip::QLineEditToolTip(QWidget * parent) : QLineEdit(parent) {

}

void QLineEditToolTip::setToolTipText( const QString& message ){
  QToolTip::showText( this->mapToGlobal( QPoint( 0, 0 ) ), message );
}
