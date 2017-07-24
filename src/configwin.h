#ifndef SRC_CONFIGWIN_H__
#define SRC_CONFIGWIN_H__

#include <QMainWindow>
#include <QtWidgets>

#include <opencv2/core/hal/interface.h>                   // for CV_8UC1
#include <opencv2/imgcodecs/imgcodecs_c.h>                // for ::CV_LOAD_I...
#include <opencv2/core.hpp>                               // for RNG
#include <opencv2/core/cvstd.inl.hpp>                     // for String::String
#include <opencv2/core/mat.hpp>                           // for Mat
#include <opencv2/core/mat.inl.hpp>                       // for Mat::Mat
#include <opencv2/core/operations.hpp>                    // for RNG::RNG
#include <opencv2/core/types.hpp>                         // for Rect, Point3d
#include <opencv2/imgcodecs.hpp>                          // for imread

/** START BOOST **/
#include <boost/process.hpp>
#include <boost/filesystem/operations.hpp>                // for directory_iterator
#include <boost/filesystem/path.hpp>                      // for path, operator==, oper...

#if defined(BOOST_POSIX_API)
#include <boost/process/detail/posix/basic_pipe.hpp>
#elif defined(BOOST_WINDOWS_API)
#include <boost/process/detail/windows/basic_pipe.hpp>
#endif

/** END BOOST **/

#include "td_map.hpp"
#include "treemodel.h"
#include "ui_configwin.h"
#include "treeitem.h"
#include "cv_image_widget.h"
#include <QToolBar>
#include <QMenu>
#include <QAction>
#include "settings.h"
#include "ui_settings.h"

#include "treeitem_file_delegate.hpp"
#include "gui_sim_updater.hpp"
#include "gui_stream.hpp"
#include "cv_image_table_frame.h"

#include "application_log.hpp"
#include "group_options.h"


namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT
  public:
    //  MainWindow(QWidget *parent = 0);
    MainWindow( ApplicationLog::ApplicationLog* logger, QWidget *parent = 0 );

    bool set_dr_probe_path( QString path );
    bool set_application_logger( ApplicationLog::ApplicationLog* logger );
    bool set_project_dir_path( boost::filesystem::path base_dir );

    void loadFile(const QString &fileName);
    ~MainWindow();

    bool checkSettings();
    boost::process::ipstream _sim_tdmap_celslc_ostream_buffer;
    boost::process::ipstream _sim_tdmap_msa_ostream_buffer;
    boost::process::ipstream _sim_tdmap_wavimg_ostream_buffer;
    boost::process::ipstream _sim_tdmap_simgrid_ostream_buffer;

    public slots:
      void update_from_TDMap_sucess();
    void update_from_TDMap_failure();
    bool _is_initialization_ok();
    void update_tdmap_celslc_started( );
    void update_tdmap_celslc_ended( bool result );
    void update_tdmap_celslc_step( int at_step );
    void update_tdmap_msa_started( );
    void update_tdmap_msa_ended( bool result );
    void update_tdmap_wavimg_started( );
    void update_tdmap_wavimg_ended( bool result );
    void update_tdmap_simgrid_started( );
    void update_tdmap_simgrid_ended( bool result );
    void update_tdmap_no_simgrid_ended( bool result );

  protected:
    void closeEvent(QCloseEvent *event) override;
    bool _was_document_modified();
    bool _reset_document_modified_flags();

    private slots:

      bool update_qline_image_path( std::string fileName );
    void update_full_experimental_image();
    void update_roi_experimental_image_frame();
    void update_roi_full_experimental_image_frame();
    void on_qpush_run_tdmap_clicked();
    void update_simgrid_frame( );
    void update_super_cell_target_region();
    void update_super_cell_target_region_shapes();
    void update_tdmap_sim_ostream_celslc();
    void update_tdmap_sim_ostream_msa();
    void update_tdmap_sim_ostream_wavimg();
    void update_tdmap_sim_ostream_simgrid();
    void clear_tdmap_sim_ostream_containers();
    void update_tdmap_best_match(int x,int y);
    void update_tdmap_current_selection(int x,int y);
    void update_from_SuperCell_edge_sucess();
    void update_from_SuperCell_edge_failure();

    // menu slots
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void about();
    void documentWasModified();
    bool export_TDMap();

    void on_qpush_apply_edge_detection_clicked();
    void on_qpush_test_tdmap_clicked();

signals:
    void experimental_image_filename_changed();
    void simulated_grid_changed( );
    void super_cell_target_region_changed();
    void force_close();

  private:

    // preferences methods
    bool maybeSetPreferences();
    bool maybeSetProject();
    bool edit_preferences();

    // menu methods
    void createActions();
    void updateProgressBar( int lower_range, int current_value, int upper_range );
    void updateProgressBar( int lower_range, int current_value, int upper_range , bool error );
    void createProgressBar();

    void create_box_options();
    bool readSettings();
    void writeSettings();
    bool maybeSave();
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    QString curFile;
    bool _flag_project_setted = false;
    Ui::MainWindow *ui;
    TDMap *_core_td_map;

    // tab 1
    TreeModel *project_setup_image_fields_model;
    TreeModel *project_setup_crystalographic_fields_model;

    // tab 2
    TreeModel *tdmap_simulation_setup_model;
    TreeModel *tdmap_running_configuration_model;

    // tab3
    TreeModel* super_cell_setup_model;

    QModelIndex project_setup_image_fields_model_index;

    /* Threads and workers */
    GuiSimOutUpdater *sim_tdmap_worker;
    QThread *_sim_tdmap_thread;

    GuiSimOutUpdater *sim_super_cell_worker;
    QThread *_sim_super_cell_thread;

    GuiSimOutUpdater *sim_tdmap_output_worker;
    QThread *_sim_tdmap_output_thread;

    /* Delegates */
    TreeItemFileDelegate *_load_file_delegate;

    /* Settings */
    QString _q_settings_fileName;
    QString _dr_probe_bin_path;
    QString _dr_probe_celslc_bin;
    QString _dr_probe_msa_bin;
    QString _dr_probe_wavimg_bin;
    bool _flag_dr_probe_bin_path = false;
    bool _flag_dr_probe_celslc_bin = false;
    bool _flag_dr_probe_msa_bin = false;
    bool _flag_dr_probe_wavimg_bin = false;

    bool _settings_ok = false;
    bool _failed_initialization = false;

    /* Loggers */
    ApplicationLog::ApplicationLog* im2model_logger = nullptr;
    bool _flag_im2model_logger = false;

    QMenu* alignMenu;
    QAction*  auto_range_lower_upper;
    QAction*  auto_lower_upper;
    CustomToolButton* alignToolButton;

/* GUI options */
  TreeItem* experimental_image_root = nullptr;
  TreeItem* image_path = nullptr;
TreeItem* experimental_sampling_rate = nullptr;
TreeItem* experimental_sampling_rate_x = nullptr;
TreeItem* experimental_sampling_rate_y = nullptr;
TreeItem* experimental_roi = nullptr;

  TreeItem* experimental_roi_center = nullptr;
  TreeItem* experimental_roi_center_x = nullptr;
  TreeItem* experimental_roi_center_y = nullptr;

  TreeItem* experimental_roi_dimensions = nullptr;
  TreeItem* experimental_roi_dimensions_width = nullptr;
  TreeItem* experimental_roi_dimensions_width_px = nullptr;
  TreeItem* experimental_roi_dimensions_width_nm = nullptr;
  TreeItem* experimental_roi_dimensions_height = nullptr;
  TreeItem* experimental_roi_dimensions_height_px = nullptr;
  TreeItem* experimental_roi_dimensions_height_nm = nullptr;
  TreeItem* crystallography_root = nullptr;
  TreeItem* unit_cell_file = nullptr;
  TreeItem* unit_cell_file_cif = nullptr;
  TreeItem* projection_direction = nullptr;
  TreeItem* projection_direction_h = nullptr;
  TreeItem* projection_direction_k = nullptr;
  TreeItem* projection_direction_l = nullptr;
  TreeItem* upward_vector = nullptr;
  TreeItem* upward_vector_u = nullptr;
  TreeItem* upward_vector_v = nullptr;
  TreeItem* upward_vector_w = nullptr;
  TreeItem* tdmap_root = nullptr;
  TreeItem* _parameter_variation_map = nullptr;
  TreeItem* _parameter_variation_map_thickness = nullptr;
  TreeItem* _parameter_variation_map_thickness_estimated_nm = nullptr;
  TreeItem* thickness_range_number_samples = nullptr;
  TreeItem* thickness_range = nullptr;
  TreeItem* thickness_range_lower_bound = nullptr;
  TreeItem* thickness_range_upper_bound = nullptr;
  TreeItem* _parameter_variation_map_defocous = nullptr;
  TreeItem* _parameter_variation_map_defocus_estimated_nm = nullptr;
  TreeItem* defocus_range_number_samples = nullptr;
  TreeItem* defocus_range = nullptr;
  TreeItem* defocus_range_lower_bound = nullptr;
  TreeItem* defocus_range_upper_bound = nullptr;
  TreeItem* incident_electron_beam = nullptr;
  TreeItem* accelaration_voltage_kv = nullptr;
  TreeItem* tdmap_cell_dimensions = nullptr;
  TreeItem* tdmap_cell_dimensions_a = nullptr;
  TreeItem* tdmap_cell_dimensions_b = nullptr;
  TreeItem* _simulation_refinement = nullptr;
  TreeItem* _aberration_parameters = nullptr;
  TreeItem* spherical_aberration_nm = nullptr;
  TreeItem* _envelope_parameters = nullptr;
  TreeItem* _envelope_parameters_vibrational_damping = nullptr;
  TreeItem* envelop_parameters_vibrational_damping_isotropic_first_rms_amplitude = nullptr;
  TreeItem* envelop_parameters_vibrational_damping_isotropic_second_rms_amplitude = nullptr;
  TreeItem* envelop_parameters_vibrational_damping_isotropic_orientation_angle = nullptr;

    /* simulation outputs */
    TreeItem* _multislice_phase_granting_output;
    TreeItem* _electron_diffraction_patterns_output;
    TreeItem* _image_intensity_distribuitions_output;
    TreeItem* _image_correlation_output;

    QModelIndex* _index_multislice_phase_granting_output;
    QModelIndex* _index_electron_diffraction_patterns_output;
    QModelIndex* _index_image_intensity_distribuitions_output;
    QModelIndex* _index_image_correlation_output;

    group_options* celslc_step_group_options;
    group_options* msa_step_group_options;
    group_options* wavimg_step_group_options;
    group_options* simgrid_step_group_options;
    group_options* super_cell_target_step_group_options;

    QProgressBar* running_progress;

};


#endif // CONFIGWIN_H
