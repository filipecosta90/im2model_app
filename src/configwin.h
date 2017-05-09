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

    void on_qline_super_cell_size_a_editingFinished();

    void on_qline_super_cell_size_b_editingFinished();

    void on_qline_super_cell_size_c_editingFinished();

    void on_qline_nx_simulated_horizontal_samples_editingFinished();

    void on_qline_ny_simulated_vertical_samples_editingFinished();

    void on_qline_slice_file_name_prefix_editingFinished();

    void on_qline_accelaration_voltage_ht_editingFinished();

    void on_qline_exp_nx_editingFinished();

    void on_qline_exp_ny_editingFinished();

    void on_qline_roi_center_x_editingFinished();

    void on_qline_roi_center_y_editingFinished();

    void on_qline_roi_size_x_editingFinished();

    void on_qline_roi_size_y_editingFinished();

  private:
    Ui::MainWindow *ui;
};

#endif // CONFIGWIN_H
