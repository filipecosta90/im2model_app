#include "configwin.h"

#include "settings.h"
#include "ui_settings.h"

#include <QFileDialog>
#include <QFileSystemModel>
#include <QTreeView>
#include <QFile>
#include <QtWidgets>

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {

  ui->setupUi(this);
  ui->td_map_splitter->setStretchFactor(0,3);
  ui->td_map_splitter->setStretchFactor(1,7);
  ui->td_map_splitter->setStretchFactor(2,2);
  createActions();
  updateStatusBar();
  setCurrentFile(QString());
  setUnifiedTitleAndToolBarOnMac(true);
  _load_file_delegate = new TreeItemFileDelegate(this);

  bool _settings_ok = readSettings();
  while ( ! _settings_ok ){
    edit_preferences();
    _settings_ok = checkSettings();
  }

  _core_image_crystal = new Image_Crystal();
  _core_td_map = new TDMap( _sim_tdmap_ostream_buffer, _core_image_crystal );
  _core_td_map->set_dr_probe_bin_path( _dr_probe_bin_path.toStdString() );
  _core_td_map->set_dr_probe_celslc_execname( _dr_probe_celslc_bin.toStdString() );
  _core_td_map->set_dr_probe_msa_execname( _dr_probe_msa_bin.toStdString() );
  _core_td_map->set_dr_probe_wavimg_execname( _dr_probe_wavimg_bin.toStdString() );

  create_box_options();

  /* TDMap simulation thread */
  _sim_tdmap_thread = new QThread( this );
  sim_tdmap_worker = new GuiSimOutUpdater( _core_td_map  );

  sim_tdmap_worker->moveToThread( _sim_tdmap_thread );

  // will only start thread when needed
  connect(sim_tdmap_worker, SIGNAL(TDMap_request()), _sim_tdmap_thread, SLOT(start()));
  connect(_sim_tdmap_thread, SIGNAL(started()), this, SLOT(update_tdmap_sim_ostream()));
  connect(_sim_tdmap_thread, SIGNAL(started()), sim_tdmap_worker, SLOT(newTDMapSim()));


  connect(sim_tdmap_worker, SIGNAL(TDMap_sucess()), this, SLOT(update_from_TDMap_sucess()));
  connect(sim_tdmap_worker, SIGNAL(TDMap_failure()), this, SLOT(update_from_TDMap_failure()));
  // will quit thread after work done
  connect(sim_tdmap_worker, SIGNAL(finished()), _sim_tdmap_thread, SLOT(quit()), Qt::DirectConnection);


  connect( ui->tdmap_table, SIGNAL(tdmap_best_match( int, int )), this, SLOT(update_tdmap_best_match(int,int)) );
  connect(ui->tdmap_table, SIGNAL(cellClicked(int , int )), this, SLOT(update_tdmap_current_selection(int,int)) );

  connect(this, SIGNAL(experimental_image_filename_changed()), this, SLOT(update_full_experimental_image_frame()));
  connect(this, SIGNAL(simulated_grid_changed()), this, SLOT(update_simgrid_frame()));

}

void MainWindow::update_tdmap_best_match(int x,int y){

  ////qDebug() << "updating tdmap best match to: " << x << " y: " << y ;
  cv::Mat _simulated_image = _core_td_map->get_simulated_image_in_grid(x,y);
  const double _simulated_image_match = _core_td_map->get_simulated_image_match_in_grid(x,y);
  const double _simulated_image_thickness = _core_td_map->get_simulated_image_thickness_nm_in_grid(x,y);
  const double _simulated_image_defocus = _core_td_map->get_simulated_image_defocus_in_grid(x,y);

  QStandardItemModel* model = new QStandardItemModel(3, 2,this);
  model->setHeaderData(0, Qt::Horizontal, tr("Parameter"));
  model->setHeaderData(1, Qt::Horizontal, tr("Value"));
  QStandardItem *label_match_item = new QStandardItem(tr("Match %"));
  QStandardItem *label_defocus_item = new QStandardItem(tr("Defocus"));
  QStandardItem *label_thickness_item = new QStandardItem(tr("Thickness"));
  QStandardItem *match_item = new QStandardItem( QString::number( _simulated_image_match ) );
  QStandardItem *defocus_item = new QStandardItem( QString::number( _simulated_image_defocus  ) );
  QStandardItem *thickness_item = new QStandardItem( QString::number( _simulated_image_thickness ) );
  model->setItem(0, 0, label_match_item);
  model->setItem(1, 0, label_thickness_item);
  model->setItem(2, 0, label_defocus_item);
  model->setItem(0, 1, match_item);
  model->setItem(1, 1, thickness_item);
  model->setItem(2, 1, defocus_item);

  ui->qgraphics_tdmap_best_match->setModel(model);
  ui->qgraphics_tdmap_best_match->setImage( _simulated_image );
}

void MainWindow::update_tdmap_current_selection(int x,int y){

  if(_core_td_map->_is_simulated_images_grid_defined()){
    ////qDebug() << "updating update_tdmap_current_selection to: " << x << " y: " << y ;
    cv::Mat _simulated_image = _core_td_map->get_simulated_image_in_grid(x,y);
    const double _simulated_image_match = _core_td_map->get_simulated_image_match_in_grid(x,y);
    const double _simulated_image_thickness = _core_td_map->get_simulated_image_thickness_nm_in_grid(x,y);
    const double _simulated_image_defocus = _core_td_map->get_simulated_image_defocus_in_grid(x,y);

    QStandardItemModel* model = new QStandardItemModel(3, 2,this);
    model->setHeaderData(0, Qt::Horizontal, tr("Parameter"));
    model->setHeaderData(1, Qt::Horizontal, tr("Value"));
    QStandardItem *label_match_item = new QStandardItem(tr("Match %"));
    QStandardItem *label_defocus_item = new QStandardItem(tr("Defocus"));
    QStandardItem *label_thickness_item = new QStandardItem(tr("Thickness"));
    QStandardItem *match_item = new QStandardItem( QString::number( _simulated_image_match ) );
    QStandardItem *defocus_item = new QStandardItem( QString::number( _simulated_image_defocus  ) );
    QStandardItem *thickness_item = new QStandardItem( QString::number( _simulated_image_thickness ) );
    model->setItem(0, 0, label_match_item);
    model->setItem(1, 0, label_thickness_item);
    model->setItem(2, 0, label_defocus_item);
    model->setItem(0, 1, match_item);
    model->setItem(1, 1, thickness_item);
    model->setItem(2, 1, defocus_item);
    ui->qgraphics_tdmap_selection->setModel(model);
    ui->qgraphics_tdmap_selection->setImage( _simulated_image );
  }
}

MainWindow::~MainWindow(){

  /* END TDMap simulation thread */
  sim_tdmap_worker->abort();
  _sim_tdmap_thread->wait();
  ////qDebug()<<"Deleting thread and worker in Thread "<<this->QObject::thread()->currentThreadId();
  delete _sim_tdmap_thread;
  delete sim_tdmap_worker;

  /* DELETE UI */
  delete ui;
}

bool MainWindow::update_qline_image_path( std::string fileName ){
  ////qDebug()<<"Inside update_qline_image_path: " << fileName ;
  _core_image_crystal->set_experimental_image_path( fileName );
  const bool load_ok = _core_image_crystal->load_full_experimental_image();
  if( load_ok ){
    emit experimental_image_filename_changed();
  }
  return true;
}

void MainWindow::update_simgrid_frame(){
  ////qDebug()<<" updating simgrid images";
  std::vector< std::vector<cv::Mat> > _simulated_images_grid = _core_td_map->get_simulated_images_grid();
  this->ui->tdmap_table->set_simulated_images_grid( _simulated_images_grid );
}

void MainWindow::update_full_experimental_image_frame(){
  cv::Mat full_image = _core_image_crystal->get_full_experimental_image_mat();
  ui->qgraphics_full_experimental_image->setImage( full_image );
  ui->qgraphics_full_experimental_image->show();
  update_roi_experimental_image_frame();
}

void MainWindow::update_roi_experimental_image_frame(){
  if( _core_image_crystal->_is_roi_defined() ){
    cv::Mat roi_image = _core_image_crystal->get_roi_experimental_image_mat();
    ui->qgraphics_roi_experimental_image->setImage( roi_image );
    ui->qgraphics_full_experimental_image->show();
  }
}

bool MainWindow::_was_document_modified(){
  bool result = false;
  result |= project_setup_image_fields_model->_was_model_modified();
  result |= project_setup_crystalographic_fields_model->_was_model_modified();
  result |= tdmap_simulation_setup_model->_was_model_modified();
  return result;
}

void   MainWindow::update_from_TDMap_sucess(){
  ////qDebug()<<"tdmap thread signaled sucess " ;
  ui->statusBar->showMessage(tr("Sucessfully runned TD-Map"), 2000);
  emit simulated_grid_changed();
}

void   MainWindow::update_from_TDMap_failure(){
  ////qDebug()<<"tdmap thread signaled failure " ;
  ui->statusBar->showMessage(tr("Error while running TD-Map"), 2000);
}

void MainWindow::update_tdmap_sim_ostream(){
  ////qDebug()<<"waiting for tdmap sim output " ;
  ui->qTextBrowser_tdmap_simulation_output->setText(" ");
  std::string line;
  while(std::getline(_sim_tdmap_ostream_buffer, line)){
    ui->qTextBrowser_tdmap_simulation_output->moveCursor (QTextCursor::End);
    QString qt_linw =  QString::fromStdString( line);
    ui->qTextBrowser_tdmap_simulation_output->append( qt_linw );
    QApplication::processEvents();
  }
}

void MainWindow::on_qpush_run_tdmap_clicked(){
  bool status = false;
  ////qDebug()<<"running tdmap" ;
  ui->statusBar->showMessage(tr("Running TD-Map"), 2000);
  ////qDebug()<< "Master thread " << QThread::currentThread() ;
  sim_tdmap_worker->requestTDMap();

}

void MainWindow::closeEvent(QCloseEvent *event)
{
  if (maybeSave()) {
    writeSettings();
    event->accept();
  } else {
    event->ignore();
  }
}

void MainWindow::newFile()
{
  if (maybeSave()) {
    setCurrentFile(QString());
  }
}

void MainWindow::open()
{
  if (maybeSave()) {
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
      loadFile(fileName);
  }
}

bool MainWindow::save()
{
  if ( curFile.isEmpty() ){
    return saveAs();
  } else {
    return saveFile(curFile);
  }
}

bool MainWindow::saveAs()
{
  QFileDialog dialog(this);
  dialog.setWindowModality(Qt::WindowModal);
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  if (dialog.exec() != QDialog::Accepted)
    return false;
  return saveFile(dialog.selectedFiles().first());
}



void MainWindow::edit_preferences(){
  QSettings settings;
  Settings dialog;
  _q_settings_fileName = settings.fileName();
  dialog.set_q_settings_fileName( _q_settings_fileName.toStdString() );
  dialog.set_dr_probe_bin_path( _dr_probe_bin_path.toStdString() );
  dialog.set_dr_probe_celslc_bin( _dr_probe_celslc_bin.toStdString() );
  dialog.set_dr_probe_msa_bin( _dr_probe_msa_bin.toStdString() );
  dialog.set_dr_probe_wavimg_bin( _dr_probe_wavimg_bin.toStdString() );
  dialog.produce_settings_panel();
  dialog.exec();
  if ( dialog._is_save_preferences() ){
    std::cout << "GOING TO SAVE PREFERENCES" << std::endl;
    _dr_probe_bin_path = dialog.get_dr_probe_bin_path();
    _dr_probe_celslc_bin = dialog.get_dr_probe_celslc_bin();
    _dr_probe_msa_bin = dialog.get_dr_probe_msa_bin();
    _dr_probe_wavimg_bin = dialog.get_dr_probe_wavimg_bin();
    writeSettings();
  }
}

void MainWindow::about()
{
  QMessageBox::about(this, tr("Im2Model"),
      tr("<b>Im2Model</b> combines transmission electron microscopy, image correlation and matching procedures,"
        "enabling the determination of a three-dimensional atomic structure based strictly on a single high-resolution experimental image."
        "Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P."));
}

void MainWindow::documentWasModified(){
  // setWindowModified(textEdit->document()->isModified());
}

void MainWindow::createActions(){
  QMenu *fileMenu = ui->menuBar->addMenu(tr("&File"));
  //QToolBar *fileToolBar = addToolBar(tr("File"));
  const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/Icons/MenuIconNew32"));
  QAction *newAct = new QAction(newIcon, tr("&New"), this);
  newAct->setShortcuts(QKeySequence::New);
  newAct->setStatusTip(tr("Create a new file"));
  connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
  fileMenu->addAction(newAct);

  const QIcon openIcon = QIcon::fromTheme("document-open", QIcon( ":/Icons/MenuIconOpen32"));
  QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
  openAct->setShortcuts(QKeySequence::Open);
  openAct->setStatusTip(tr("Open an existing file"));
  connect(openAct, &QAction::triggered, this, &MainWindow::open);
  fileMenu->addAction(openAct);

  const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/Icons/MenuIconSave32"));
  QAction *saveAct = new QAction(saveIcon, tr("&Save"), this);
  saveAct->setShortcuts(QKeySequence::Save);
  saveAct->setStatusTip(tr("Save the document to disk"));
  connect(saveAct, &QAction::triggered, this, &MainWindow::save);
  fileMenu->addAction(saveAct);

  const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
  QAction *saveAsAct = fileMenu->addAction(saveAsIcon, tr("Save &As..."), this, &MainWindow::saveAs);
  saveAsAct->setShortcuts(QKeySequence::SaveAs);
  saveAsAct->setStatusTip(tr("Save the document under a new name"));

  fileMenu->addSeparator();

  const QIcon exitIcon = QIcon::fromTheme("application-exit");
  QAction *exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), this, &QWidget::close);
  exitAct->setShortcuts(QKeySequence::Quit);
  exitAct->setStatusTip(tr("Exit the application"));

  QMenu *editMenu = ui->menuBar->addMenu(tr("&Edit"));
  QAction* preferencesAct = editMenu->addAction(tr("&Preferences"), this, &MainWindow::edit_preferences);
  //preferencesAct->setShortcuts(QKeySequence::UnknownKey);

  editMenu->addAction(preferencesAct);

  QMenu *helpMenu = ui->menuBar->addMenu(tr("&Help"));
  QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
  aboutAct->setStatusTip(tr("Show the application's About box"));
}

void MainWindow::updateStatusBar(){
  ui->statusBar->showMessage(tr("Ready"));
}

bool MainWindow::checkSettings(){
  bool status = true;
  status &= _flag_dr_probe_bin_path;

  //if its defined lets check if exists
  if( _flag_dr_probe_bin_path ){
    bool dr_probe_path_exists = false;
    boost::filesystem::path bin_dir( _dr_probe_bin_path.toStdString() );
    if( boost::filesystem::is_directory( bin_dir ) ){
      dr_probe_path_exists = true;
    }
    status &= dr_probe_path_exists;
  }

  // check if the bins are not equal to ""
  _flag_dr_probe_celslc_bin &= ( _dr_probe_celslc_bin == QString("") ) ? false : true;
  _flag_dr_probe_msa_bin &= ( _dr_probe_msa_bin == QString("") ) ? false : true;
  _flag_dr_probe_wavimg_bin &= ( _dr_probe_wavimg_bin == QString("") ) ? false : true;

  status &= _flag_dr_probe_celslc_bin;
  status &= _flag_dr_probe_msa_bin;
  status &= _flag_dr_probe_wavimg_bin;

  return status;
}

bool MainWindow::readSettings(){
  QSettings settings;

  settings.beginGroup("DrProbe");

  _flag_dr_probe_bin_path =  (settings.childKeys().contains("path", Qt::CaseInsensitive)) ? true : false;
  _flag_dr_probe_celslc_bin =  (settings.childKeys().contains("celslc", Qt::CaseInsensitive)) ? true : false;
  _flag_dr_probe_msa_bin =  (settings.childKeys().contains("msa", Qt::CaseInsensitive)) ? true : false;
  _flag_dr_probe_wavimg_bin =  (settings.childKeys().contains("wavimg", Qt::CaseInsensitive)) ? true : false;

  _q_settings_fileName = settings.fileName();
  _dr_probe_bin_path = settings.value("path","").toString();
  _dr_probe_celslc_bin = settings.value("celslc","").toString();
  _dr_probe_msa_bin = settings.value("msa","").toString();
  _dr_probe_wavimg_bin = settings.value("wavimg","").toString();

  settings.endGroup();

  return checkSettings();
}

void MainWindow::writeSettings(){
  QSettings settings;
  settings.beginGroup("DrProbe");
  settings.setValue("path",_dr_probe_bin_path);
  settings.setValue("celslc",_dr_probe_celslc_bin);
  settings.setValue("msa",_dr_probe_msa_bin);
  settings.setValue("wavimg",_dr_probe_wavimg_bin);
  settings.endGroup();
}

bool MainWindow::maybeSave(){
  if ( ! _was_document_modified() ){
    return true;
  }
  const QMessageBox::StandardButton ret
    = QMessageBox::warning(this, tr("Application"),
        tr("The document has been modified.\n"
          "Do you want to save your changes?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
  switch (ret) {
    case QMessageBox::Save:
      return save();
    case QMessageBox::Cancel:
      return false;
    default:
      break;
  }
  return true;
}

void MainWindow::loadFile(const QString &fileName){

#ifndef QT_NO_CURSOR
  QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

  // The first parameter in the constructor is the character used for indentation, whilst the second is the indentation length.
  boost::property_tree::xml_writer_settings<std::string> pt_settings( ' ', 4 );
  boost::property_tree::ptree config;

  boost::property_tree::read_xml( fileName.toStdString(), config);

  boost::property_tree::ptree project_setup_image_fields_ptree = config.get_child("project_setup_image_fields_ptree");
  project_setup_image_fields_model->load_data_from_property_tree( project_setup_image_fields_ptree );

  boost::property_tree::ptree project_setup_crystalographic_fields_ptree = config.get_child("project_setup_crystalographic_fields_ptree");
  project_setup_crystalographic_fields_model->load_data_from_property_tree( project_setup_crystalographic_fields_ptree );

  boost::property_tree::ptree tdmap_simulation_setup_ptree = config.get_child("tdmap_simulation_setup_ptree");
  tdmap_simulation_setup_model->load_data_from_property_tree( tdmap_simulation_setup_ptree );
  // Write the property tree to the XML file.

#ifndef QT_NO_CURSOR
  QApplication::restoreOverrideCursor();
#endif

  this->setCurrentFile(fileName);
  ui->statusBar->showMessage(tr("File loaded"), 2000);
}

bool MainWindow::saveFile(const QString &fileName){

#ifndef QT_NO_CURSOR
  QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

  // The first parameter in the constructor is the character used for indentation, whilst the second is the indentation length.
  boost::property_tree::xml_writer_settings<std::string> pt_settings(' ', 4);
  boost::property_tree::ptree *project_setup_image_fields_ptree = project_setup_image_fields_model->save_data_into_property_tree();
  boost::property_tree::ptree *project_setup_crystalographic_fields_ptree = project_setup_crystalographic_fields_model->save_data_into_property_tree();
  boost::property_tree::ptree *tdmap_simulation_setup_ptree = tdmap_simulation_setup_model->save_data_into_property_tree();

  boost::property_tree::ptree *config = new boost::property_tree::ptree();

  config->add_child("project_setup_image_fields_ptree", *project_setup_image_fields_ptree);
  config->add_child("project_setup_crystalographic_fields_ptree", *project_setup_crystalographic_fields_ptree);
  config->add_child("tdmap_simulation_setup_ptree", *tdmap_simulation_setup_ptree);

  // Write the property tree to the XML file.
  boost::property_tree::write_xml( fileName.toStdString(), *config, std::locale(), pt_settings );

#ifndef QT_NO_CURSOR
  QApplication::restoreOverrideCursor();
#endif

  setCurrentFile(fileName);
  ui->statusBar->showMessage(tr("File saved"), 2000);
  return true;
}

void MainWindow::setCurrentFile(const QString &fileName){
  curFile = fileName;
  // textEdit->document()->setModified(false);
  setWindowModified(false);

  QString shownName = curFile;
  if (curFile.isEmpty()){
    shownName = "untitled.im2model";
  }
  this->setWindowFilePath(shownName);
}

QString MainWindow::strippedName(const QString &fullFileName){
  return QFileInfo(fullFileName).fileName();
}

void MainWindow::create_box_options(){

  QVector<QVariant> common_header = {"Field","Value"};

  /*************************
   * EXPERIMENTAL IMAGE
   *************************/
  TreeItem* experimental_image_root = new TreeItem ( common_header );

  ////////////////
  // Image Path
  ////////////////
  QVector<QVariant> box1_option_1 = {"Image path",""};
  QVector<bool> box1_option_1_edit = {false,true};
  boost::function<bool(std::string)> box1_function_1( boost::bind( &MainWindow::update_qline_image_path, this, _1 ) );
  TreeItem* image_path  = new TreeItem (  box1_option_1 , box1_function_1, box1_option_1_edit );
  image_path->set_item_delegate_type( _delegate_FILE );
  experimental_image_root->insertChildren( image_path );

  ////////////////
  // Sampling rate
  ////////////////
  QVector<QVariant> box1_option_2 = {"Sampling (nm/pixel)",""};
  QVector<QVariant> box1_option_2_1 = {"x",""};
  QVector<bool> box1_option_2_1_edit = {false,true};
  boost::function<bool(std::string)> box1_function_2_1 ( boost::bind( &Image_Crystal::set_experimental_sampling_x,_core_image_crystal, _1 ) );
  QVector<QVariant> box1_option_2_2 = {"y",""};
  QVector<bool> box1_option_2_2_edit = {false,true};
  boost::function<bool(std::string)> box1_function_2_2 ( boost::bind( &Image_Crystal::set_experimental_sampling_y,_core_image_crystal, _1 ) );

  TreeItem* experimental_sampling_rate = new TreeItem ( box1_option_2  );
  TreeItem* experimental_sampling_rate_x = new TreeItem ( box1_option_2_1 , box1_function_2_1, box1_option_2_1_edit );
  TreeItem* experimental_sampling_rate_y = new TreeItem ( box1_option_2_2 , box1_function_2_2, box1_option_2_2_edit );
  experimental_image_root->insertChildren( experimental_sampling_rate );
  experimental_sampling_rate->insertChildren( experimental_sampling_rate_x );
  experimental_sampling_rate->insertChildren( experimental_sampling_rate_y );

  ////////////////
  // ROI
  ////////////////
  QVector<QVariant> box1_option_3 = {"ROI",""};
  TreeItem* experimental_roi = new TreeItem ( box1_option_3  );
  experimental_image_root->insertChildren( experimental_roi );

  ////////////////
  // ROI Center
  ////////////////
  QVector<QVariant> box1_option_3_1 = {"Center",""};
  QVector<QVariant> box1_option_3_1_1 = {"x",""};
  QVector<bool> box1_option_3_1_1_edit = {false,true};
  boost::function<bool(std::string)> box1_function_3_1_1 ( boost::bind( &Image_Crystal::set_experimental_roi_center_x,_core_image_crystal, _1 ) );
  QVector<QVariant> box1_option_3_1_2 = {"y",""};
  QVector<bool> box1_option_3_1_2_edit = {false,true};
  boost::function<bool(std::string)> box1_function_3_1_2 ( boost::bind( &Image_Crystal::set_experimental_roi_center_y,_core_image_crystal, _1 ) );

  TreeItem* experimental_roi_center = new TreeItem ( box1_option_3_1  );
  TreeItem* experimental_roi_center_x = new TreeItem ( box1_option_3_1_1 , box1_function_3_1_1, box1_option_3_1_1_edit );
  connect( experimental_roi_center_x, SIGNAL(dataChanged( int )), this, SLOT( update_roi_experimental_image_frame() ) );

  TreeItem* experimental_roi_center_y = new TreeItem ( box1_option_3_1_2 , box1_function_3_1_2, box1_option_3_1_2_edit );
  connect( experimental_roi_center_y, SIGNAL(dataChanged( int )), this, SLOT( update_roi_experimental_image_frame() ) );

  experimental_roi->insertChildren( experimental_roi_center );
  experimental_roi_center->insertChildren( experimental_roi_center_x );
  experimental_roi_center->insertChildren( experimental_roi_center_y );

  ////////////////
  // ROI Dimensions
  ////////////////

  QVector<QVariant> box1_option_3_2 = {"Dimensions",""};
  QVector<QVariant> box1_option_3_2_1 = {"width",""};
  QVector<bool> box1_option_3_2_1_edit = {false,true};
  boost::function<bool(std::string)> box1_function_3_2_1 ( boost::bind( &Image_Crystal::set_experimental_roi_dimensions_width,_core_image_crystal, _1 ) );
  QVector<QVariant> box1_option_3_2_2 = {"height",""};
  QVector<bool> box1_option_3_2_2_edit = {false,true};
  boost::function<bool(std::string)> box1_function_3_2_2 ( boost::bind( &Image_Crystal::set_experimental_roi_dimensions_height,_core_image_crystal, _1 ) );

  TreeItem* experimental_roi_dimensions = new TreeItem ( box1_option_3_2  );
  TreeItem* experimental_roi_dimensions_width = new TreeItem ( box1_option_3_2_1 , box1_function_3_2_1, box1_option_3_2_1_edit );
  connect( experimental_roi_dimensions_width, SIGNAL(dataChanged( int )), this, SLOT( update_roi_experimental_image_frame() ) );

  TreeItem* experimental_roi_dimensions_height = new TreeItem ( box1_option_3_2_2 , box1_function_3_2_2, box1_option_3_2_2_edit );
  connect( experimental_roi_dimensions_height, SIGNAL(dataChanged( int )), this, SLOT( update_roi_experimental_image_frame() ) );

  experimental_roi->insertChildren( experimental_roi_dimensions );
  experimental_roi_dimensions->insertChildren( experimental_roi_dimensions_width );
  experimental_roi_dimensions->insertChildren( experimental_roi_dimensions_height );

  project_setup_image_fields_model = new TreeModel( experimental_image_root );

  ui->qtree_view_project_setup_image->setModel(project_setup_image_fields_model);
  ui->qtree_view_project_setup_image->setItemDelegate( _load_file_delegate );
  //start editing after one click
  ui->qtree_view_project_setup_image->setEditTriggers(QAbstractItemView::AllEditTriggers);

  /*************************
   * CRYSTALLOGRAPLY
   *************************/
  TreeItem* crystallography_root = new TreeItem ( common_header );

  ////////////////
  // Unit-cell file
  ////////////////
  QVector<QVariant> box2_option_1 = {"Unit-cell file",""};
  TreeItem* unit_cell_file  = new TreeItem ( box2_option_1 );
  crystallography_root->insertChildren( unit_cell_file );

  ////////////////
  // Unit-cell file CIF
  ////////////////
  QVector<QVariant> box2_option_1_1 = {"CIF",""};
  QVector<bool> box2_option_1_1_edit = {false,true};
  boost::function<bool(std::string)> box2_function_1_1 ( boost::bind( &Image_Crystal::set_unit_cell_cif_path,_core_image_crystal, _1 ) );
  TreeItem* unit_cell_file_cif = new TreeItem ( box2_option_1_1 , box2_function_1_1, box2_option_1_1_edit );
  unit_cell_file_cif->set_item_delegate_type( _delegate_FILE );
  unit_cell_file->insertChildren( unit_cell_file_cif );

  ////////////////
  // Unit-cell file CEL
  ////////////////
  QVector<QVariant> box2_option_1_2 = {"CEL",""};
  QVector<bool> box2_option_1_2_edit = {false,true};
  boost::function<bool(std::string)> box2_function_1_2 ( boost::bind( &Image_Crystal::set_unit_cell_cel_path,_core_image_crystal, _1 ) );
  TreeItem* unit_cell_file_cel = new TreeItem ( box2_option_1_2 , box2_function_1_2, box2_option_1_2_edit );
  unit_cell_file_cel->set_item_delegate_type( _delegate_FILE );
  unit_cell_file->insertChildren( unit_cell_file_cel );

  ////////////////
  // Projected y axis
  ////////////////
  QVector<QVariant> box2_option_2 = {"Projected y axis",""};
  TreeItem* projected_y_axis  = new TreeItem (  box2_option_2 );
  crystallography_root->insertChildren( projected_y_axis );

  ////////////////
  //Projected y axis u
  ////////////////
  QVector<QVariant> box2_option_2_1 = {"u",""};
  QVector<bool> box2_option_2_1_edit = {false,true};
  boost::function<bool(std::string)> box2_function_2_1 ( boost::bind( &Image_Crystal::set_projected_y_axis_u,_core_image_crystal, _1 ) );
  TreeItem* projected_y_axis_u = new TreeItem ( box2_option_2_1 , box2_function_2_1, box2_option_2_1_edit );
  projected_y_axis->insertChildren( projected_y_axis_u );

  ////////////////
  //Projected y axis v
  ////////////////
  QVector<QVariant> box2_option_2_2 = {"v",""};
  QVector<bool> box2_option_2_2_edit = {false,true};
  boost::function<bool(std::string)> box2_function_2_2 ( boost::bind( &Image_Crystal::set_projected_y_axis_v,_core_image_crystal, _1 ) );
  TreeItem* projected_y_axis_v = new TreeItem ( box2_option_2_2 , box2_function_2_2, box2_option_2_2_edit );
  projected_y_axis->insertChildren( projected_y_axis_v );

  ////////////////
  //Projected y axis w
  ////////////////
  QVector<QVariant> box2_option_2_3 = {"w",""};
  QVector<bool>  box2_option_2_3_edit = {false,true};
  boost::function<bool(std::string)>  box2_function_2_3 ( boost::bind( &Image_Crystal::set_projected_y_axis_w,_core_image_crystal, _1 ) );
  TreeItem* projected_y_axis_w = new TreeItem (  box2_option_2_3 ,  box2_function_2_3,  box2_option_2_3_edit );
  projected_y_axis->insertChildren( projected_y_axis_w );

  ////////////////
  // Projection direction
  ////////////////
  QVector<QVariant> box2_option_3 = {"Projection direction",""};
  TreeItem* projection_direction  = new TreeItem (  box2_option_3 );
  crystallography_root->insertChildren( projection_direction );

  ////////////////
  // Projection direction h
  ////////////////
  QVector<QVariant> box2_option_3_1 = {"h",""};
  QVector<bool> box2_option_3_1_edit = {false,true};
  boost::function<bool(std::string)> box2_function_3_1 ( boost::bind( &Image_Crystal::set_projection_direction_h,_core_image_crystal, _1 ) );
  TreeItem* projection_direction_h = new TreeItem ( box2_option_3_1 , box2_function_3_1, box2_option_3_1_edit );
  projection_direction->insertChildren( projection_direction_h );

  ////////////////
  // Projection direction k
  ////////////////
  QVector<QVariant> box2_option_3_2 = {"k",""};
  QVector<bool> box2_option_3_2_edit = {false,true};
  boost::function<bool(std::string)> box2_function_3_2 ( boost::bind( &Image_Crystal::set_projection_direction_k,_core_image_crystal, _1 ) );
  TreeItem* projection_direction_k = new TreeItem ( box2_option_3_2 , box2_function_3_2, box2_option_3_2_edit );
  projection_direction->insertChildren( projection_direction_k );

  ////////////////
  // Projection direction l
  ////////////////
  QVector<QVariant> box2_option_3_3 = {"l",""};
  QVector<bool> box2_option_3_3_edit = {false,true};
  boost::function<bool(std::string)> box2_function_3_3 ( boost::bind( &Image_Crystal::set_projection_direction_l,_core_image_crystal, _1 ) );
  TreeItem* projection_direction_l = new TreeItem ( box2_option_3_3 , box2_function_3_3, box2_option_3_3_edit );
  projection_direction->insertChildren( projection_direction_l );


  project_setup_crystalographic_fields_model = new TreeModel( crystallography_root );

  ui->qtree_view_project_setup_crystallography->setModel(project_setup_crystalographic_fields_model);
  ui->qtree_view_project_setup_crystallography->setItemDelegate( _load_file_delegate );
  //start editing after one click
  ui->qtree_view_project_setup_crystallography->setEditTriggers(QAbstractItemView::AllEditTriggers);

  ui->qtree_view_project_setup_image->expandAll();
  ui->qtree_view_project_setup_crystallography->expandAll();

  for (int column = 0; column < project_setup_image_fields_model->columnCount(); ++column){
    ui->qtree_view_project_setup_image->resizeColumnToContents(column);
  }

  for (int column = 0; column < project_setup_crystalographic_fields_model->columnCount(); ++column){
    ui->qtree_view_project_setup_crystallography->resizeColumnToContents(column);
  }

  /*************************
   * TD MAP
   *************************/
  TreeItem* tdmap_root = new TreeItem ( common_header );

  ////////////////
  // Thickness range
  ////////////////
  QVector<QVariant> box3_option_1 = {"Thickness range",""};
  TreeItem* thickness_range  = new TreeItem ( box3_option_1 );
  tdmap_root->insertChildren( thickness_range );

  ////////////////
  //Thickness range -- lower bound
  ////////////////
  QVector<QVariant> box3_option_1_1 = {"Lower bound",""};
  QVector<bool> box3_option_1_1_edit = {false,true};
  boost::function<bool(std::string)> box3_function_1_1 ( boost::bind( &TDMap::set_thickness_range_lower_bound, _core_td_map, _1 ) );
  TreeItem* thickness_range_lower_bound = new TreeItem ( box3_option_1_1 , box3_function_1_1, box3_option_1_1_edit );
  thickness_range->insertChildren( thickness_range_lower_bound );

  ////////////////
  //Thickness range -- upper bound
  ////////////////
  QVector<QVariant> box3_option_1_2 = {"Upper bound",""};
  QVector<bool> box3_option_1_2_edit = {false,true};
  boost::function<bool(std::string)> box3_function_1_2 ( boost::bind( &TDMap::set_thickness_range_upper_bound, _core_td_map, _1 ) );
  TreeItem* thickness_range_upper_bound = new TreeItem ( box3_option_1_2 , box3_function_1_2, box3_option_1_2_edit );
  thickness_range->insertChildren( thickness_range_upper_bound );

  ////////////////
  //Thickness range -- Samples
  ////////////////
  QVector<QVariant> box3_option_1_3 = {"Samples",""};
  QVector<bool> box3_option_1_3_edit = {false,true};
  boost::function<bool(std::string)> box3_function_1_3 ( boost::bind( &TDMap::set_thickness_range_number_samples, _core_td_map, _1 ) );
  TreeItem* thickness_range_number_samples = new TreeItem ( box3_option_1_3 , box3_function_1_3, box3_option_1_3_edit );
  thickness_range->insertChildren( thickness_range_number_samples );

  ////////////////
  // Defocus range
  ////////////////
  QVector<QVariant> box3_option_2 = {"Defocus range",""};
  TreeItem* defocus_range  = new TreeItem ( box3_option_2 );
  tdmap_root->insertChildren( defocus_range );

  ////////////////
  //Defocus range -- lower bound
  ////////////////
  QVector<QVariant> box3_option_2_1 = {"Lower bound",""};
  QVector<bool> box3_option_2_1_edit = {false,true};
  boost::function<bool(std::string)> box3_function_2_1 ( boost::bind( &TDMap::set_defocus_range_lower_bound, _core_td_map, _1 ) );
  TreeItem* defocus_range_lower_bound = new TreeItem ( box3_option_2_1 , box3_function_2_1, box3_option_2_1_edit );
  defocus_range->insertChildren( defocus_range_lower_bound );

  ////////////////
  //Defocus range -- upper bound
  ////////////////
  QVector<QVariant> box3_option_2_2 = {"Upper bound",""};
  QVector<bool> box3_option_2_2_edit = {false,true};
  boost::function<bool(std::string)> box3_function_2_2 ( boost::bind( &TDMap::set_defocus_range_upper_bound, _core_td_map, _1 ) );
  TreeItem* defocus_range_upper_bound = new TreeItem ( box3_option_2_2 , box3_function_2_2, box3_option_2_2_edit );
  defocus_range->insertChildren( defocus_range_upper_bound );

  ////////////////
  //Defocus range -- Samples
  ////////////////
  QVector<QVariant> box3_option_2_3 = {"Samples",""};
  QVector<bool> box3_option_2_3_edit = {false,true};
  boost::function<bool(std::string)> box3_function_2_3 ( boost::bind( &TDMap::set_defocus_range_number_samples, _core_td_map, _1 ) );
  TreeItem* defocus_range_number_samples = new TreeItem ( box3_option_2_3 , box3_function_2_3, box3_option_2_3_edit );
  defocus_range->insertChildren( defocus_range_number_samples );

  ////////////////
  // Incident electron beam
  ////////////////
  QVector<QVariant> box3_option_3 = {"Incident electron beam",""};
  TreeItem* incident_electron_beam  = new TreeItem ( box3_option_3 );
  tdmap_root->insertChildren( incident_electron_beam );

  ////////////////
  // Incident electron beam -- Accelaration voltage (kV)
  ////////////////
  QVector<QVariant> box3_option_3_1 = {"Accelaration voltage (kV)",""};
  QVector<bool> box3_option_3_1_edit = {false,true};
  boost::function<bool(std::string)> box3_function_3_1 ( boost::bind( &TDMap::set_accelaration_voltage_kv, _core_td_map, _1 ) );
  TreeItem* accelaration_voltage_kv = new TreeItem ( box3_option_3_1 , box3_function_3_1, box3_option_3_1_edit );
  incident_electron_beam->insertChildren( accelaration_voltage_kv );

  tdmap_simulation_setup_model = new TreeModel( tdmap_root );

  ui->qtree_view_tdmap_simulation_setup->setModel( tdmap_simulation_setup_model );
  //start editing after one click
ui->qtree_view_tdmap_simulation_setup->setEditTriggers(QAbstractItemView::AllEditTriggers);

  ui->qtree_view_tdmap_simulation_setup->expandAll();
  for (int column = 0; column < tdmap_simulation_setup_model->columnCount(); ++column){
    ui->qtree_view_tdmap_simulation_setup->resizeColumnToContents(column);
  }

  ui->tdmap_table->set_tdmap( _core_td_map );
  ui->tdmap_table->connect_thickness_range_number_samples_changes(  thickness_range_number_samples, 1 );
  ui->tdmap_table->connect_defocus_range_number_samples_changes(  defocus_range_number_samples, 1 );

  /*
     simulation_parameters << tr(" [] Import fixed values from refinement?");
     simulation_parameters << tr("Advanced options");
     simulation_parameters << tr(" Multislice phase grating");
     simulation_parameters << tr("  Slice filename prefix\tslc");
     simulation_parameters << tr("  Super-Cell Slicing along c-axis");
     simulation_parameters << tr("   .\t");
     simulation_parameters << tr("   .\t");
     simulation_parameters << tr("   .\t0");
     simulation_parameters << tr("  Optional Parameters");
     simulation_parameters << tr("   Apply random atomic displacements");
     simulation_parameters << tr("   Thermal vibration models");
     simulation_parameters << tr("    .");
     simulation_parameters << tr("    .");
     simulation_parameters << tr(" Electron diffraction patterns");
     simulation_parameters << tr("  Input Wavefunction file name\t");
     simulation_parameters << tr(" Image intensity distribuitions");
     simulation_parameters << tr(" Running configurations");
     simulation_parameters << tr("  Compute multislice phase grating");
     simulation_parameters << tr("  Compute electron diffraction patterns");
     simulation_parameters << tr("  Compute image intensity distribuitions");
     simulation_parameters << tr("  Compute TD map");




  //tdmap
  QModelIndex tdmap_index = simulation_parameters_model->index(0,0);

  ui->qtree_view_simulation_setup->expand(tdmap_index);



  QStringList headers_edge_detection_parameters;
  headers_edge_detection_parameters << tr("Field") << tr("Value");

  QStringList edge_detection_parameters;
  edge_detection_parameters << tr("Edge detection");
  edge_detection_parameters << tr(" Hysteresis Thresholding");
  edge_detection_parameters << tr("Max contour distance (pixels)");

  TreeModel *edge_detection_model = new TreeModel(headers_edge_detection_parameters, edge_detection_parameters);
  ui->qtree_view_supercell_model_edge_detection_setup->setModel(edge_detection_model);

  for (int column = 0; column < edge_detection_model->columnCount(); ++column){
  ui->qtree_view_supercell_model_edge_detection_setup->resizeColumnToContents(column);
  }


  QStringList headers_supercell_model_refinement_parameters;
  headers_supercell_model_refinement_parameters << tr("Field") << tr("Value");

  QStringList supercell_model_refinement_parameters;
  supercell_model_refinement_parameters << tr("Supercell file");
  supercell_model_refinement_parameters << tr(" CIF");
  supercell_model_refinement_parameters << tr(" CEL");
  supercell_model_refinement_parameters << tr("Edge refinement");
  supercell_model_refinement_parameters << tr("Columns Refinement");
  supercell_model_refinement_parameters << tr("Calculation");
  TreeModel *supercell_model_refinement_model = new TreeModel(headers_supercell_model_refinement_parameters, supercell_model_refinement_parameters);

  ui->qtree_view_supercell_model_refinement_setup->setModel(supercell_model_refinement_model);

  for (int column = 0; column < supercell_model_refinement_model->columnCount(); ++column){
  ui->qtree_view_supercell_model_refinement_setup->resizeColumnToContents(column);
  }*/

  // ui->qtree_view_project_setup_image->setModel(project_setup_image_fields_model);
  // ui->qtree_view_project_setup_image->setItemDelegate( _load_file_delegate );
}

bool MainWindow::set_dr_probe_path( QString path ){
  _dr_probe_bin_path = path;
  return true;
}
