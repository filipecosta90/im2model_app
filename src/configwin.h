/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#ifndef SRC_CONFIGWIN_H__
#define SRC_CONFIGWIN_H__

#include <QMainWindow>
#include <QtWidgets>

#include <Qt3DRender/qcamera.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DRender/qcameralens.h>

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QCommandLinkButton>
#include <QtGui/QScreen>
#include <QToolBar>
#include <QMenu>
#include <QAction>
#include <Qt3DInput/QInputAspect>
#include <QFrameGraphNode>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QLayer>
#include <Qt3DRender/QLayerFilter>
#include <Qt3DRender/QViewport>

#include <Qt3DExtras/qtorusmesh.h>
#include <Qt3DRender/qmesh.h>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qeffect.h>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qsceneloader.h>
#include <Qt3DRender/qpointlight.h>

#include <Qt3DCore/qtransform.h>
#include <Qt3DCore/qaspectengine.h>
#include <QSceneLoader>
#include <QTechniqueFilter>
#include <QViewport>
#include <QClearBuffers>
#include <QCameraSelector>
#include <QRenderPassFilter>
#include <QSceneLoader>

#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DExtras/qforwardrenderer.h>

#include <Qt3DExtras/qt3dwindow.h>
#include <Qt3DExtras/qfirstpersoncameracontroller.h>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DExtras/QPhongMaterial>
#include <QTreeView>

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
#include "cv_image_frame.h"
#include "qt_scene_supercell.h"
#include "settings.h"
#include "ui_settings.h"

#include "treeitem_file_delegate.hpp"
#include "gui_sim_updater.hpp"
#include "gui_stream.hpp"
#include "cv_image_table_frame.h"

#include "application_log.hpp"
#include "group_options.h"

#include "vis/inputcontrols/qtrackballcameracontroller.h"

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT
  public:
    //  MainWindow(QWidget *parent = 0);
    MainWindow( ApplicationLog::ApplicationLog* logger, QWidget *parent = 0 );

    bool set_dr_probe_path( QString path );
    void setApplicationVersion( std::string app_version );
    bool set_application_logger( ApplicationLog::ApplicationLog* logger );
    bool set_project_dir_path( boost::filesystem::path base_dir );

    void loadFile(const QString &fileName);
    ~MainWindow();

    bool checkSettings();
    boost::process::ipstream _sim_tdmap_celslc_ostream_buffer;
    boost::process::ipstream _sim_tdmap_msa_ostream_buffer;
    boost::process::ipstream _sim_tdmap_wavimg_ostream_buffer;
    boost::process::ipstream _sim_tdmap_simgrid_ostream_buffer;
    boost::process::ipstream _sim_supercell_celslc_ostream_buffer;
    boost::process::ipstream _sim_supercell_msa_ostream_buffer;
    boost::process::ipstream _sim_supercell_wavimg_ostream_buffer;

    public slots:
      // void echo_sc();
      void update_from_TDMap_sucess();
    void update_from_TDMap_failure();
    bool _is_initialization_ok();
    void update_tdmap_celslc_started( );

    void update_tdmap_celslc_started_with_steps_info( int n_steps );
    void update_supercell_celslc_started_with_steps_info( int n_steps );
    void update_supercell_celslc_ssc_single_slice_step( bool step_result );
    void update_tdmap_celslc_ssc_single_slice_step( bool step_result );

    void update_supercell_celslc_started( );
    void update_tdmap_celslc_ended( bool result );
    void update_supercell_celslc_ended( bool result );
    void update_tdmap_celslc_step( int at_step );
    void update_tdmap_msa_started( );
    void update_supercell_msa_started( );
    void update_tdmap_msa_ended( bool result );
    void update_supercell_msa_ended( bool result );
    void update_tdmap_wavimg_started( );
    void update_supercell_wavimg_started( );
    void update_tdmap_wavimg_ended( bool result );
    void update_supercell_wavimg_ended( bool result );
    void update_tdmap_simgrid_started( );
    void update_tdmap_simgrid_ended( bool result );
    void update_tdmap_no_simgrid_ended( bool result );
    void update_exp_image_roi_from_rectangle_selection( QRect );
    void update_tab3_exp_image_bounds_from_rectangle_selection( QRect rectangle_selection );
    void update_exp_image_properties_roi_rectangle_statistical_from_rectangle_selection( QRect rectangle_selection );
    void update_from_full_SuperCell_sucess();
    void update_from_full_SuperCell_failure();
    void update_from_full_SuperCell_intensity_cols_sucess();
    void update_from_full_SuperCell_intensity_cols_failure();
    void full_simulation_intensity_columns_SelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

  protected:
    void closeEvent(QCloseEvent *event) override;
    bool _was_document_modified();
    bool _reset_document_modified_flags();

    private slots:
      bool update_qline_image_path( std::string fileName );
    bool update_qline_cif_path( std::string fileName );
    bool update_qline_mtf_path( std::string fileName );
    void update_full_experimental_image();
    void update_roi_experimental_image_frame();
    void update_roi_full_experimental_image_frame();
    void update_super_cell_experimental_image_intensity_columns();
    void update_super_cell_simulated_image_intensity_columns();
    void update_super_cell_simulated_image_full_image();
    void update_super_cell_simulated_image_roi_image();
    void on_qpush_run_tdmap_clicked();
    void update_simgrid_frame( );
    bool copy_external_dependencies();
    bool copy_external_files_to_project_dir();
    void update_super_cell_target_region();
    void update_super_cell_target_region_image();
    void update_super_cell_target_region_shapes();
    void update_tdmap_sim_ostream_celslc();
    void update_tdmap_sim_ostream_msa();
    void update_tdmap_sim_ostream_wavimg();
    void update_tdmap_sim_ostream_simgrid();
    void clear_tdmap_sim_ostream_containers();
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
    bool export_TDMap_cutted();
    bool export_TDMap_full();
    bool export_TDMap( bool cut_margin = false );
    bool export_current_simulated_image_selection( int x, int y );
    bool export_current_simulated_image_overlay_selection( int x, int y );
    bool export_IntegratedIntensities_onlymapped();
    bool export_IntegratedIntensities_full();
    bool export_IntegratedIntensities( bool onlymapped = true );

    void on_qpush_apply_edge_detection_clicked();
    void on_qpush_test_tdmap_clicked();
    void on_qbutton_tdmap_accept_clicked();
    void on_qpush_compute_full_super_cell_clicked();
    void on_qpush_run_compute_intensity_columns_clicked();

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
    bool initialize();

    // menu methods
    void createActions();
    void updateProgressBar( int lower_range, int current_value, int upper_range );
    void updateProgressBar( int lower_range, int current_value, int upper_range , bool error );
    void createProgressBar();

    bool create_3d_widgets( QMainWindow *parent , SuperCell* tdmap_roi_sim_super_cell, SuperCell* tdmap_full_sim_super_cell );

    void create_box_options_tab1_exp_image();
    void create_box_options_tab1_crystallography();
    void create_box_options_tab2_sim_config();
    void create_box_options_tab2_run_config();
    void create_box_options_tab3_supercell();
    void create_box_options_tab4_intensity_peaks();
    void create_box_options_tab4_intensity_columns_listing();
    void create_box_options();

    bool readSettings();
    void writeSettings();
    bool maybeSave();
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    std::string application_version;
    bool  _flag_application_version = false;

    QString curFile;
    bool _flag_project_setted = false;
    Ui::MainWindow *ui;
    TDMap *_core_td_map;
    int _core_td_map_info_celslc_n_steps = 1;
    int _core_td_map_info_celslc_at_step = 0;
    int _core_td_map_info_supercell_celslc_n_steps = 1;
    int _core_td_map_info_supercell_celslc_at_step = 0;

    // tab 1
    TreeModel *project_setup_image_fields_model;
    TreeModel *project_setup_crystalographic_fields_model;
    TreeModel *atom_info_fields_model;

    // tab 2
    TreeModel *tdmap_simulation_setup_model;
    TreeModel *tdmap_running_configuration_model;
    cv::Point2i tdmap_current_selection_pos;

    // tab3
    TreeModel* super_cell_setup_model;

    // tab4
    TreeModel* intensity_peaks_model;
    TreeModel* intensity_columns_listing_model;

    TreeModel* chem_database_model;

    QModelIndex project_setup_image_fields_model_index;

    /* Threads and workers */
    GuiSimOutUpdater *sim_tdmap_worker;
    QThread *_sim_tdmap_thread;

    GuiSimOutUpdater *sim_super_cell_worker;
    QThread *_sim_super_cell_thread;

    GuiSimOutUpdater *full_sim_super_cell_worker;
    QThread *full_sim_super_cell_thread;

    GuiSimOutUpdater *full_sim_super_cell_intensity_cols_worker;
    QThread *full_sim_super_cell_intensity_cols_thread;


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
    TreeItem* chem_database_root = nullptr;

    TreeItem* experimental_image_root = nullptr;
    TreeItem* experimental_sampling_rate = nullptr;
    TreeItem* experimental_sampling_rate_x = nullptr;
    TreeItem* image_path = nullptr;
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
    TreeItem* unit_cell_display = nullptr;
    TreeItem* unit_cell_display_expand_factor = nullptr;
    TreeItem* unit_cell_display_expand_factor_a = nullptr;
    TreeItem* unit_cell_display_expand_factor_b = nullptr;
    TreeItem* unit_cell_display_expand_factor_c = nullptr;
    TreeItem* zone_axis = nullptr;
    TreeItem* zone_axis_u = nullptr;
    TreeItem* zone_axis_v = nullptr;
    TreeItem* zone_axis_w = nullptr;
    TreeItem* upward_vector = nullptr;
    TreeItem* upward_vector_u = nullptr;
    TreeItem* upward_vector_v = nullptr;
    TreeItem* upward_vector_w = nullptr;
    TreeItem* orientation_matrix = nullptr;

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
    TreeItem* partial_temporal_coherence_focus_spread = nullptr;
    TreeItem* partial_spatial_coherence_semi_convergence_angle = nullptr;
    TreeItem* _mtf_parameters = nullptr;
    TreeItem* image_correlation = nullptr;
    TreeItem* image_correlation_matching_method = nullptr;
    TreeItem* image_normalization_method = nullptr;

    // tab 3
    TreeItem* super_cell_setup_root = nullptr;
    TreeItem* super_cell_edge_detection = nullptr;
    TreeItem* edge_detection_hysteris_thresholding = nullptr;
    TreeItem* edge_detection_max_contour_distance = nullptr;
    TreeItem* super_cell_dimensions = nullptr;
    TreeItem* super_cell_dimensions_a = nullptr;
    TreeItem* super_cell_dimensions_b = nullptr;
    TreeItem* super_cell_dimensions_c = nullptr;
    TreeItem* super_cell_rectangle_selection = nullptr;
    TreeItem* noise_carbon_roi_statistical_analysis = nullptr;
    TreeItem* exp_image_properties_noise_carbon_statistical_mean = nullptr;
    TreeItem* exp_image_properties_noise_carbon_statistical_stddev = nullptr;

    // tab 4 - 1
    TreeItem* intensity_peaks_root = nullptr;
    TreeItem* intensity_peaks_analysis = nullptr;
    TreeItem* intensity_peaks_display = nullptr;
    TreeItem* intensity_peaks_display_simulated_img = nullptr;
    TreeItem* intensity_peaks_display_simulated_img_alpha = nullptr;
    TreeItem* intensity_peaks_display_simulated_img_alpha_channel = nullptr;
    TreeItem* intensity_peaks_display_simulated_img_distance_transform_alpha = nullptr;
    TreeItem* intensity_peaks_display_experimental_img = nullptr;
    TreeItem* intensity_peaks_display_experimental_img_alpha = nullptr;
    TreeItem* intensity_peaks_display_experimental_img_alpha_channel = nullptr;
    TreeItem* intensity_peaks_display_experimental_img_distance_transform_alpha = nullptr;

    // tab 4 - 2
    TreeItem* intensity_columns_listing_root = nullptr;
    TreeItem* super_cell_sim_image_intensity_columns = nullptr;

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
