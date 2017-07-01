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
