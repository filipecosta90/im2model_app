#ifndef CONFIGWIN_H
#define CONFIGWIN_H

#include <QMainWindow>
#include "image_crystal.hpp"
#include "td_map.hpp"

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT
  public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

  private slots:
    void on_qpush_load_image_clicked();

    void on_qpush_load_cif_clicked();

    void on_actionAbout_triggered();

  private:
    Ui::MainWindow *ui;
    Image_Crystal *image_crystal;
    TDMap *td_map;

};

#endif // CONFIGWIN_H
