/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#ifndef SRC_QDOUBLEVALIDATOR_H__
#define SRC_QDOUBLEVALIDATOR_H__
/**
 * The class QDoubleValidatorReporter extends QDoubleValidator with the
 * possibility to report through a signal whether the current value has
 * been validated as valid or not.
 */

#include <iostream>
#include <QDoubleValidator>

class QDoubleValidatorReporter : public QDoubleValidator {
  Q_OBJECT;

  public:
  /**
   * @see QIntValidator::QDoubleValidator(QObject * parent = 0)
   */
  QDoubleValidatorReporter(QObject * parent = 0);

  // @Override
  virtual QValidator::State validate(QString& input, int& pos ) const;

signals:
  /**
   * This signal is emitted whenever the string is validated.
   * If the string is valid, an empty string is passed.
   * If the string is not valid, an appropriate error message is passed.
   */
  void setError(const QString& msg) const;
};

#endif
