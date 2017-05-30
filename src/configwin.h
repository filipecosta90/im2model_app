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

#include <boost/process.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem/operations.hpp>                // for directory_iterator
#include <boost/filesystem/path.hpp>                      // for path, operator==, oper...
#include <boost/process.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/asio/buffer.hpp>


#include <boost/filesystem/operations.hpp>                // for directory_iterator
#include <boost/filesystem/path.hpp>                      // for path, operator==, oper...
#include <boost/process.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/asio/buffer.hpp>

#include <boost/process/error.hpp>
#include <boost/process/async.hpp>
#include <boost/process/io.hpp>
#include <boost/process/child.hpp>


#include <boost/system/error_code.hpp>

#include <boost/asio.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <boost/process/error.hpp>
#include <boost/process/io.hpp>
#include <boost/process/args.hpp>
#include <boost/process/child.hpp>
#include <boost/process/async_pipe.hpp>
#include <system_error>

#include <boost/filesystem.hpp>

#include <boost/thread.hpp>
#include <future>


#include "image_crystal.hpp"
#include "td_map.hpp"
#include "treemodel.h"
#include "ui_configwin.h"
#include "treeitem.h"
#include "cv_image_widget.h"
#include "treeitem_file_delegate.hpp"
#include "gui_sim_updater.hpp"
#include "gui_stream.hpp"

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT
  public:
    explicit MainWindow(QWidget *parent = 0);

    void loadFile(const QString &fileName);
    ~MainWindow();

    boost::process::ipstream _sim_tdmap_ostream_buffer;
    public slots:
    void update_from_TDMap_sucess();
    void update_from_TDMap_failure();


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

  private:

    // menu methods
    void createActions();
    void updateStatusBar();
    void create_box_options();
    void readSettings();
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
    QModelIndex project_setup_image_fields_model_index;
    QString m_sSettingsFile;

    QString _dr_probe_bin_path;
    QString _dr_probe_celslc_bin;
    QString _dr_probe_msa_bin;
    QString _dr_probe_wavimg_bin;

    /* Threads and workers */
    GuiSimOutUpdater *sim_tdmap_worker;
    QThread *_sim_tdmap_thread;
    std::ostream* _sim_tdmap_thread_ostream;

    GuiSimOutUpdater *sim_tdmap_output_worker;
    QThread *_sim_tdmap_output_thread;

    /* Delegates */
    TreeItemFileDelegate *_load_file_delegate;


};

#endif // CONFIGWIN_H
