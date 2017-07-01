#ifndef SRC_QINTVALIDATOR_H__
#define SRC_QINTVALIDATOR_H__
/**
 * The class QIntValidatorReporter extends QIntValidator with the
 * possibility to report through a signal whether the current value has
 * been validated as valid or not.
 */

#include <iostream>
#include <QIntValidator>

class QIntValidatorReporter : public QIntValidator {
  Q_OBJECT;

  public:
  /**
   * @see QIntValidator::QIntValidator(int minimum, int maximum, QObject * parent = 0)
   */
  QIntValidatorReporter(QObject * parent = 0);

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
