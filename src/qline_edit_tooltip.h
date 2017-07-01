#ifndef SRC_QLINEEDITTOOLTIP_H__
#define SRC_QLINEEDITTOOLTIP_H__
/**
 * The class QIntValidatorReporter extends QIntValidator with the
 * possibility to report through a signal whether the current value has
 * been validated as valid or not.
 */

#include <iostream>
#include <QIntValidator>
#include <QLineEdit>
#include <QToolTip>


class QLineEditToolTip : public QLineEdit {
  Q_OBJECT;

  public:
  QLineEditToolTip(QWidget * parent = 0);

public slots:
void setToolTipText(const QString&);
};

#endif
