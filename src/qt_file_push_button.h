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
