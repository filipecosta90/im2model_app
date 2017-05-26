#include "qt_file_push_button.h"
#include <iostream>

FilePushButton::FilePushButton( QWidget *parent ):
  QPushButton( parent )
{
  _button_parent = parent;
  connect( this, SIGNAL( clicked()),this,SLOT( __click() ));
}

void FilePushButton::__click()
{
  emit onClick( _button_parent );
}
