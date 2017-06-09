#ifndef CONFIGWIN_H
#define CONFIGWIN_H

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
/** END BOOST **/

#include "image_crystal.hpp"
#include "td_map.hpp"
#include "treemodel.h"
#include "ui_configwin.h"
#include "treeitem.h"
#include "cv_image_widget.h"
#include <QToolBar>
#include <QMenu>
#include <QAction>

#include "treeitem_file_delegate.hpp"
#include "gui_sim_updater.hpp"
#include "gui_stream.hpp"
#include "cv_image_table_frame.h"
#include "settings.h"
#include "ui_settings.h"
#include "application_log.hpp"


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
    void set_base_dir_path( boost::filesystem::path base_dir );

    void loadFile(const QString &fileName);
    ~MainWindow();

    bool checkSettings();

    boost::process::ipstream _sim_tdmap_ostream_buffer;
    public slots:
      void update_from_TDMap_sucess();
    void update_from_TDMap_failure();
    bool _is_initialization_ok();

  protected:
    void closeEvent(QCloseEvent *event) override;
    bool _was_document_modified();

    private slots:

      bool update_qline_image_path( std::string fileName );
    void update_full_experimental_image_frame();
    void update_roi_experimental_image_frame();
    void on_qpush_run_tdmap_clicked();
    void update_simgrid_frame();
    void update_tdmap_sim_ostream();
    void update_tdmap_best_match(int x,int y);
    void update_tdmap_current_selection(int x,int y);

    // menu slots
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void about();
    void documentWasModified();

signals:
    void experimental_image_filename_changed( );
    void simulated_grid_changed();
    void force_close();

  private:

    // preferences methods
    bool maybeSetPreferences();
    bool edit_preferences();

    // menu methods
    void createActions();
    void updateStatusBar();
    void create_box_options();
    bool readSettings();
    void writeSettings();
    bool maybeSave();
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    QString curFile;
    Ui::MainWindow *ui;
    Image_Crystal *_core_image_crystal;
    TDMap *_core_td_map;

    TreeModel *project_setup_image_fields_model;
    TreeModel *project_setup_crystalographic_fields_model;
    TreeModel *tdmap_simulation_setup_model;
    TreeModel *tdmap_running_configuration_model;

    QModelIndex project_setup_image_fields_model_index;

    /* Threads and workers */
    GuiSimOutUpdater *sim_tdmap_worker;
    QThread *_sim_tdmap_thread;
    std::ostream* _sim_tdmap_thread_ostream;

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

    /* Base dir path */
    boost::filesystem::path base_dir_path;
    bool _flag_base_dir_path = false;

    QMenu* alignMenu;
    QAction*  auto_range_lower_upper;
    QAction*  auto_lower_upper;
    CustomToolButton* alignToolButton;

    /* simulation outputs */
    TreeItem* _multislice_phase_granting_output;
    TreeItem* _electron_diffraction_patterns_output;
    TreeItem* _image_intensity_distribuitions_output;
    TreeItem* _image_correlation_output;

    QModelIndex* _index_multislice_phase_granting_output;
    QModelIndex* _index_electron_diffraction_patterns_output;
    QModelIndex* _index_image_intensity_distribuitions_output;
    QModelIndex* _index_image_correlation_output;


};

#endif // CONFIGWIN_H
