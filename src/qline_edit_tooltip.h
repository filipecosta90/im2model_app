/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

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
