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
#include "qdouble_validator.h"

QDoubleValidatorReporter::QDoubleValidatorReporter(QObject * parent) : QDoubleValidator(parent) {

}

QValidator::State QDoubleValidatorReporter::validate(QString& input, int& pos ) const {
  // use the normal validate
  QValidator::State result = QDoubleValidator::validate(input, pos);

  if (result == QValidator::Invalid) {
    emit setError(QString("Allowed range: %1 to %2").arg(bottom()).arg(top()));
  }
  return result;
}

