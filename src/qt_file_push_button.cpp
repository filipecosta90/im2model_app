/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#include "qt_file_push_button.h"
#include <iostream>

FilePushButton::FilePushButton( QString text, QWidget *parent ): QPushButton( text, parent )
{
  _button_parent = parent;
  connect( this, SIGNAL( clicked()),this,SLOT( __click() ));
}

FilePushButton::FilePushButton( QWidget *parent ): QPushButton( parent )
{
  _button_parent = parent;
  connect( this, SIGNAL( clicked()),this,SLOT( __click() ));
}

void FilePushButton::__click()
{
  emit onClick( _button_parent );
}
