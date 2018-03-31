/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#ifndef CUSTOMTOOLBUTTON_H
#define CUSTOMTOOLBUTTON_H

#include <QToolButton>
#include <QDebug>

class CustomToolButton : public QToolButton
{
  Q_OBJECT
  public:
    explicit CustomToolButton(QWidget *parent = 0);
};

#endif // CUSTOMTOOLBUTTON_H

