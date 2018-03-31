/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#include "custom_tool_button.h"

CustomToolButton::CustomToolButton(QWidget *parent) :
  QToolButton(parent)
{
  setPopupMode(QToolButton::MenuButtonPopup);
  QObject::connect(this, SIGNAL(triggered(QAction*)),
      this, SLOT(setDefaultAction(QAction*)));
}
