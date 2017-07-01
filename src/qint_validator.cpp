/* StackOverflow reference:
 * http://stackoverflow.com/questions/21390859/is-it-possible-for-qvalidator-to-show-a-popup
 */

#include <QDebug>
#include "qint_validator.h"


QIntValidatorReporter::QIntValidatorReporter(QObject * parent) : QIntValidator(parent) {

}

QValidator::State QIntValidatorReporter::validate(QString& input, int& pos ) const {
  // use the normal validate
  QValidator::State result = QIntValidator::validate(input, pos);

  if (result == QValidator::Invalid) {
    emit setError(QString("Allowed range: %1 to %2").arg(bottom()).arg(top()));
  }
  return result;
}
