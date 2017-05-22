#ifndef CONFIGWIN_H
#define CONFIGWIN_H

#include <QMainWindow>

#include <opencv2/core/hal/interface.h>                   // for CV_8UC1
#include <opencv2/imgcodecs/imgcodecs_c.h>                // for ::CV_LOAD_I...
#include <opencv2/core.hpp>                               // for RNG
#include <opencv2/core/cvstd.inl.hpp>                     // for String::String
#include <opencv2/core/mat.hpp>                           // for Mat
#include <opencv2/core/mat.inl.hpp>                       // for Mat::Mat
#include <opencv2/core/operations.hpp>                    // for RNG::RNG
#include <opencv2/core/types.hpp>                         // for Rect, Point3d
#include <opencv2/imgcodecs.hpp>                          // for imread

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
    void on_qpush_load_cel_clicked();
    void on_actionAbout_triggered();
    void update_full_experimental_image_frame();
    void update_roi_experimental_image_frame();
    void on_qpush_run_tdmap_clicked();

signals:
    void experimental_image_filename_changed( );

  private:
    Ui::MainWindow *ui;
    Image_Crystal *_core_image_crystal;
    TDMap *_core_td_map;

};

#endif // CONFIGWIN_H
