/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#ifndef FILEPUSHBUTTON_H
#define FILEPUSHBUTTON_H

#include <QPushButton>
#include <QWidget>

class FilePushButton : public QPushButton{
  Q_OBJECT
  public:
    FilePushButton( QWidget *parent = 0 );
    FilePushButton( QString text , QWidget *parent = 0 );

    virtual ~FilePushButton() {}

    public slots:
      void __click();

signals:
    void onClick( QWidget *widget );

  private:
    QWidget *_button_parent;
};

#endif // end FILEDELEGATE_H
