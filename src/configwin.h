#ifndef CONFIGWIN_H
#define CONFIGWIN_H

#include <QMainWindow>

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_qpush_load_image_clicked();

    void on_qpush_load_cif_clicked();

    void on_qpush_load_cel_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // CONFIGWIN_H
