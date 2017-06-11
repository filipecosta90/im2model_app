#include "configwin.h"

MainWindow::MainWindow( ApplicationLog::ApplicationLog* logger , QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  im2model_logger = logger;
  _flag_im2model_logger = true;
  im2model_logger->logEvent(ApplicationLog::notification, "Application logger setted for MainWindow class.");

  if (_flag_im2model_logger) {
    im2model_logger->logEvent(ApplicationLog::normal, "Setting up UI.");
  }
  ui->setupUi(this);
  ui->td_map_splitter->setStretchFactor(0,3);
  ui->td_map_splitter->setStretchFactor(1,7);
  ui->td_map_splitter->setStretchFactor(2,2);

  if (_flag_im2model_logger) {
    im2model_logger->logEvent(ApplicationLog::normal, "Creating actions.");
  }
  createActions();

  if (_flag_im2model_logger) {
    im2model_logger->logEvent(ApplicationLog::normal, "Updating status bar.");
  }
  updateStatusBar();
  setCurrentFile(QString());
  setUnifiedTitleAndToolBarOnMac(true);

  if (_flag_im2model_logger) {
    im2model_logger->logEvent(ApplicationLog::normal, "Loading file delegate.");
  }
  _load_file_delegate = new TreeItemFileDelegate(this);

  if (_flag_im2model_logger) {
    im2model_logger->logEvent(ApplicationLog::normal, "Going to read settings.");
  }
  _settings_ok = readSettings();
  if( ! _settings_ok ){
    if( _flag_im2model_logger ){
      im2model_logger->logEvent( ApplicationLog::critical, "Preferences are not setted. trying to resolve." );
    }
    _settings_ok = maybeSetPreferences();
  }
  /* if the preferences are still not correct set the flag */
  if ( !_settings_ok ){
    _failed_initialization = true;
    if( _flag_im2model_logger ){
      im2model_logger->logEvent( ApplicationLog::critical, "Failed to initialize preferences." );
    }
  }
  else{
    _core_image_crystal = new Image_Crystal();
    _core_td_map = new TDMap(
        _sim_tdmap_celslc_ostream_buffer,
        _sim_tdmap_msa_ostream_buffer,
        _sim_tdmap_wavimg_ostream_buffer,
        _sim_tdmap_simgrid_ostream_buffer,
        _core_image_crystal );

    if (_flag_im2model_logger) {
      im2model_logger->logEvent(ApplicationLog::critical, "Trying to set application logger for TDMap.");
      _core_td_map->set_application_logger(im2model_logger);
    }

    bool status = true;
    status &= _core_td_map->set_dr_probe_bin_path( _dr_probe_bin_path.toStdString() );
    status &= _core_td_map->set_dr_probe_celslc_execname( _dr_probe_celslc_bin.toStdString() );
    status &= _core_td_map->set_dr_probe_msa_execname( _dr_probe_msa_bin.toStdString() );
    status &= _core_td_map->set_dr_probe_wavimg_execname( _dr_probe_wavimg_bin.toStdString() );

    if( status == false ){
      _failed_initialization = true;
      if( _flag_im2model_logger ){
        im2model_logger->logEvent( ApplicationLog::critical, "Failed setting dr probe bins." );
      }
    }
    else {
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

      if( _flag_im2model_logger ){
        im2model_logger->logEvent( ApplicationLog::notification, "Finished initializing App." );
      }

    }
  }
}

void MainWindow::set_base_dir_path( boost::filesystem::path base_dir ){
  base_dir_path = base_dir;
  _flag_base_dir_path = true;
  _core_td_map->set_base_dir_path( base_dir );
}

bool MainWindow::set_application_logger( ApplicationLog::ApplicationLog* logger ){
  im2model_logger = logger;
  _flag_im2model_logger = true;
  im2model_logger->logEvent( ApplicationLog::notification, "Application logger setted for MainWindow class." );
  _core_td_map->set_application_logger( logger );
  return true;
}

bool MainWindow::_is_initialization_ok(){
  return !_failed_initialization;
}

bool MainWindow::maybeSetPreferences(){
  const QMessageBox::StandardButton ret
    = QMessageBox::warning(this, tr("Application"),
        tr("The saved prefences file is incomplete.\n""To use Im2Model all preferences vars must be set.\n"
          "Do you want to open the preferences panel?"),
        QMessageBox::Yes | QMessageBox::Close);
  switch (ret) {
    case QMessageBox::Yes:
      return edit_preferences();
    case QMessageBox::Close:
      return false;
    default:
      break;
  }
  return false;
}

void MainWindow::update_tdmap_best_match( int x,int y ){
  if(_core_td_map->_is_simulated_images_grid_defined()){
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
}

void MainWindow::update_tdmap_current_selection(int x,int y){
  if(_core_td_map->_is_simulated_images_grid_defined()){
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
  /* check if the initialization process was done */
  if( !_failed_initialization ){
    /* END TDMap simulation thread */
    sim_tdmap_worker->abort();
    _sim_tdmap_thread->wait();
    ////qDebug()<<"Deleting thread and worker in Thread "<<this->QObject::thread()->currentThreadId();
    delete _sim_tdmap_thread;
    delete sim_tdmap_worker;
  }
  /* DELETE UI */
  delete ui;
}

bool MainWindow::update_qline_image_path( std::string fileName ){
  _core_image_crystal->set_experimental_image_path( fileName );
  const bool load_ok = _core_image_crystal->load_full_experimental_image();
  if( load_ok ){
    emit experimental_image_filename_changed();
  }
  return true;
}

void MainWindow::update_simgrid_frame(){
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
  if( _settings_ok ){
    result |= project_setup_image_fields_model->_was_model_modified();
    result |= project_setup_crystalographic_fields_model->_was_model_modified();
    result |= tdmap_simulation_setup_model->_was_model_modified();
    result |= tdmap_running_configuration_model->_was_model_modified();
  }
  return result;
}

void MainWindow::update_from_TDMap_sucess(){
  ui->statusBar->showMessage(tr("Sucessfully runned TD-Map"), 2000);
  emit simulated_grid_changed();
}

void MainWindow::update_from_TDMap_failure(){
  ui->statusBar->showMessage(tr("Error while running TD-Map"), 2000);
}

void MainWindow::update_tdmap_sim_ostream(){
  std::string line;

  if( _core_td_map->get_run_celslc_switch() ){
    if( _core_td_map->_is_sim_tdmap_celslc_ostream_buffer_active() ){

      QModelIndex celslc_index = tdmap_running_configuration_model->index(1,0);
      QModelIndex celslc_out_legend_index = tdmap_running_configuration_model->index(0,0,celslc_index);
      QModelIndex celslc_out_index = project_setup_crystalographic_fields_model->index(0,1,celslc_out_legend_index);
      std::cout << "####celslc_out_index  VALID: " << celslc_out_index.isValid() << std::endl;

      while(std::getline(_sim_tdmap_celslc_ostream_buffer, line)){
        //   ui->qtree_view_tdmap_running_configuration-
        //ui->qTextBrowser_tdmap_simulation_output->moveCursor (QTextCursor::End);
        QString qt_linw =  QString::fromStdString( line );
        QVariant _new_line_var = QVariant::fromValue(qt_linw + "\n");
        bool result = tdmap_running_configuration_model->appendData( celslc_out_index, _new_line_var );
        ui->qtree_view_tdmap_simulation_setup->update();
        std::cout << "append result " << result << std::endl;
        //  _multislice_phase_granting_output->appendData( 1, _new_line_var );
        //ui->qTextBrowser_tdmap_simulation_output->append( qt_linw );
        QApplication::processEvents();
      }
      // reset pipe
      _sim_tdmap_celslc_ostream_buffer.pipe( boost::process::pipe() );
    }
  }

  if( _core_td_map->get_run_msa_switch() ){
    if( _core_td_map->_is_sim_tdmap_msa_ostream_buffer_active() ){

      QModelIndex msa_index = tdmap_running_configuration_model->index(2,0);
      QModelIndex msa_out_legend_index = tdmap_running_configuration_model->index(0,0,msa_index);
      QModelIndex msa_out_index = project_setup_crystalographic_fields_model->index(0,1,msa_out_legend_index);
      std::cout << "####msa_out_index  VALID: " << msa_out_index.isValid() << std::endl;

      while(std::getline(_sim_tdmap_msa_ostream_buffer, line)){
        QString qt_linw =  QString::fromStdString( line );
        QVariant _new_line_var = QVariant::fromValue(qt_linw + "\n");
        bool result = tdmap_running_configuration_model->appendData( msa_out_index, _new_line_var );
        ui->qtree_view_tdmap_simulation_setup->update();
        std::cout << "append result " << result << std::endl;
        QApplication::processEvents();
      }
      // reset pipe
      _sim_tdmap_msa_ostream_buffer.pipe( boost::process::pipe() );
    }
  }

  if( _core_td_map->get_run_wavimg_switch() ){
    if( _core_td_map->_is_sim_tdmap_wavimg_ostream_buffer_active() ){

      QModelIndex wavimg_index = tdmap_running_configuration_model->index(3,0);
      QModelIndex wavimg_out_legend_index = tdmap_running_configuration_model->index(0,0,wavimg_index);
      QModelIndex wavimg_out_index = project_setup_crystalographic_fields_model->index(0,1,wavimg_out_legend_index);
      std::cout << "####wavimg_out_index  VALID: " << wavimg_out_index.isValid() << std::endl;

      while(std::getline(_sim_tdmap_wavimg_ostream_buffer, line)){
        QString qt_linw =  QString::fromStdString( line );
        QVariant _new_line_var = QVariant::fromValue(qt_linw + "\n");
        bool result = tdmap_running_configuration_model->appendData( wavimg_out_index, _new_line_var );
        ui->qtree_view_tdmap_simulation_setup->update();
        std::cout << "append result " << result << std::endl;
        QApplication::processEvents();
      }
      // reset pipe
      _sim_tdmap_wavimg_ostream_buffer.pipe( boost::process::pipe() );
    }
  }

  if( _core_td_map->get_run_simgrid_switch() ){
    if( _core_td_map->_is_sim_tdmap_simgrid_ostream_buffer_active() ){

      QModelIndex simgrid_index = tdmap_running_configuration_model->index(2,0);
      QModelIndex simgrid_out_legend_index = tdmap_running_configuration_model->index(0,0,simgrid_index);
      QModelIndex simgrid_out_index = project_setup_crystalographic_fields_model->index(0,1,simgrid_out_legend_index);
      std::cout << "####simgrid_out_index  VALID: " << simgrid_out_index.isValid() << std::endl;
      // reset pipe
      _sim_tdmap_simgrid_ostream_buffer.pipe( boost::process::pipe() );
    }
  }
}

void MainWindow::on_qpush_run_tdmap_clicked(){
  bool status = false;
  ui->statusBar->showMessage(tr("Running TD-Map"), 2000);
  sim_tdmap_worker->requestTDMap();
}

void MainWindow::closeEvent(QCloseEvent *event){
  if (maybeSave()) {
    writeSettings();
    event->accept();
  } else {
    event->ignore();
  }
}

void MainWindow::newFile(){
  if (maybeSave()) {
    setCurrentFile(QString());
  }
}

void MainWindow::open(){
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


bool MainWindow::export_TDMap(){
  // returns false if no settings were saved
  bool result = false;
  if( _core_td_map ){
    if( _core_td_map->_is_simulated_images_grid_defined() ){
      QFileDialog dialog(this);
      dialog.setWindowModality(Qt::WindowModal);
      std::string preset_filename = _core_td_map->get_export_sim_grid_filename_hint();

      boost::filesystem::path filename( preset_filename );
      boost::filesystem::path full_path_filename = base_dir_path / preset_filename ;
      dialog.selectFile( QString::fromStdString(full_path_filename.string() ) );

      dialog.setAcceptMode(QFileDialog::AcceptSave);
      if (dialog.exec() != QDialog::Accepted){
        result = false;
      }
      else{
        QString tdmap_filename = dialog.selectedFiles().first();
        result = _core_td_map->export_sim_grid(  tdmap_filename.toStdString() );
      }
    }
  }
  return result;
}


bool MainWindow::edit_preferences(){
  // returns false if no settings were saved
  bool result = false;
  QSettings settings;
  Settings dialog;
  dialog.setWindowTitle ( "Configurations panel" );
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
    result = checkSettings();
  }
  return result;
}

void MainWindow::about(){
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
  preferencesAct->setShortcuts(QKeySequence::UnknownKey);
  editMenu->addAction(preferencesAct);

  QMenu *helpMenu = ui->menuBar->addMenu(tr("&Help"));
  QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
  aboutAct->setStatusTip(tr("Show the application's About box"));

  QMenu *tdmapMenu = ui->menuBar->addMenu(tr("&TD Map"));
  //QToolBar *fileToolBar = addToolBar(tr("File"));
  QAction *exportTDMap = tdmapMenu->addAction( tr("&Export"), this , &MainWindow::export_TDMap );
  // newAct->setShortcuts(QKeySequence::);
  newAct->setStatusTip(tr("Export the current TD Map"));
  tdmapMenu->addAction(exportTDMap);

}

void MainWindow::updateStatusBar(){
  ui->statusBar->showMessage(tr("Ready"));
}

bool MainWindow::checkSettings(){
  bool status = true;
  status &= _flag_dr_probe_bin_path;
  bool dr_probe_path_exists = false;
  bool _temp_flag_dr_probe_celslc_bin = _flag_dr_probe_celslc_bin;
  bool _temp_flag_dr_probe_msa_bin = _flag_dr_probe_msa_bin;
  bool _temp_flag_dr_probe_wavimg_bin = _flag_dr_probe_wavimg_bin;

  if (_flag_im2model_logger) {
    std::stringstream message;
    message << " MainWindow::checkSettings()";
    im2model_logger->logEvent(ApplicationLog::notification, message.str());
    message = std::stringstream();
    message << "_flag_dr_probe_celslc_bin: " << _flag_dr_probe_celslc_bin;
    im2model_logger->logEvent(ApplicationLog::notification, message.str());
    message = std::stringstream();
    message << "_flag_dr_probe_msa_bin: " << _flag_dr_probe_msa_bin;
    im2model_logger->logEvent(ApplicationLog::notification, message.str());
    message = std::stringstream();
    message << "_flag_dr_probe_wavimg_bin: " << _flag_dr_probe_wavimg_bin;
    im2model_logger->logEvent(ApplicationLog::notification, message.str());
    message = std::stringstream();
    message << "_flag_dr_probe_bin_path: " << _flag_dr_probe_bin_path;
    im2model_logger->logEvent(ApplicationLog::notification, message.str());
  }

  //if its defined lets check if exists
  if( _flag_dr_probe_bin_path ){
    boost::filesystem::path bin_dir( _dr_probe_bin_path.toStdString() );
    if( boost::filesystem::is_directory( bin_dir ) ){
      dr_probe_path_exists = true;
    }
    status &= dr_probe_path_exists;
  }

  // check if the bins are not equal to ""
  _temp_flag_dr_probe_celslc_bin &= (_dr_probe_celslc_bin == QString("")) ? false : true;
  _temp_flag_dr_probe_msa_bin &= (_dr_probe_msa_bin == QString("")) ? false : true;
  _temp_flag_dr_probe_wavimg_bin &= (_dr_probe_wavimg_bin == QString("")) ? false : true;

  if (_temp_flag_dr_probe_celslc_bin) {
    boost::filesystem::path bin_dir(_dr_probe_bin_path.toStdString());
    boost::filesystem::path filename(_dr_probe_celslc_bin.toStdString());
    boost::filesystem::path full_path_filename = bin_dir / filename; 
    if (boost::filesystem::exists(full_path_filename)) {
      _temp_flag_dr_probe_celslc_bin = true;
    }
    else {
      _temp_flag_dr_probe_celslc_bin = false;
    }
  }
  if (_temp_flag_dr_probe_msa_bin) {
    boost::filesystem::path bin_dir(_dr_probe_bin_path.toStdString());
    boost::filesystem::path filename(_dr_probe_msa_bin.toStdString());
    boost::filesystem::path full_path_filename = bin_dir / filename;
    if (boost::filesystem::exists(full_path_filename)) {
      _temp_flag_dr_probe_msa_bin = true;
    }
    else {
      _temp_flag_dr_probe_msa_bin = false;
    }
  }

  if (_temp_flag_dr_probe_wavimg_bin) {
    boost::filesystem::path bin_dir(_dr_probe_bin_path.toStdString());
    boost::filesystem::path filename(_dr_probe_celslc_bin.toStdString());
    boost::filesystem::path full_path_filename = bin_dir / filename;
    if (boost::filesystem::exists(full_path_filename)) {
      _temp_flag_dr_probe_wavimg_bin = true;
    }
    else {
      _temp_flag_dr_probe_wavimg_bin = false;
    }
  }

  status &= _temp_flag_dr_probe_celslc_bin;
  status &= _temp_flag_dr_probe_msa_bin;
  status &= _temp_flag_dr_probe_wavimg_bin;

  if (_flag_im2model_logger) {
    std::stringstream message;
    message << "_flag_dr_probe_celslc_bin: " << _temp_flag_dr_probe_celslc_bin;
    im2model_logger->logEvent(ApplicationLog::notification, message.str());
    message = std::stringstream();
    message << "_flag_dr_probe_msa_bin: " << _temp_flag_dr_probe_msa_bin;
    im2model_logger->logEvent(ApplicationLog::notification, message.str());
    message = std::stringstream();
    message << "_flag_dr_probe_wavimg_bin: " << _temp_flag_dr_probe_wavimg_bin;
    im2model_logger->logEvent(ApplicationLog::notification, message.str());
    message = std::stringstream();
    message << "_flag_dr_probe_bin_path: " << _flag_dr_probe_bin_path;
    im2model_logger->logEvent(ApplicationLog::notification, message.str());
    message = std::stringstream();
    message << "dr_probe_path_exists: " << dr_probe_path_exists;
    im2model_logger->logEvent(ApplicationLog::notification, message.str());
  }

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
  restoreGeometry(settings.value("geometry.main_window").toByteArray());
  ui->td_map_splitter->restoreGeometry( settings.value("geometry.tdmap_splitter").toByteArray() );
  ui->td_map_splitter->restoreState( settings.value("state.tdmap_splitter").toByteArray() );
  restoreState(settings.value("windowState").toByteArray());
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
  settings.setValue("geometry.main_window", saveGeometry() );
  settings.setValue("geometry.tdmap_splitter", ui->td_map_splitter->saveGeometry() );
  settings.setValue("state.tdmap_splitter", ui->td_map_splitter->saveState() );
  settings.setValue("windowState", saveState() );
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
  ui->qtree_view_project_setup_image->update();

  boost::property_tree::ptree project_setup_crystalographic_fields_ptree = config.get_child("project_setup_crystalographic_fields_ptree");
  project_setup_crystalographic_fields_model->load_data_from_property_tree( project_setup_crystalographic_fields_ptree );
  ui->qtree_view_project_setup_crystallography->update();

  boost::property_tree::ptree tdmap_simulation_setup_ptree = config.get_child("tdmap_simulation_setup_ptree");
  tdmap_simulation_setup_model->load_data_from_property_tree( tdmap_simulation_setup_ptree );
  ui->qtree_view_tdmap_simulation_setup->update();

  boost::property_tree::ptree tdmap_running_configuration_ptree = config.get_child("tdmap_running_configuration_ptree");
  tdmap_running_configuration_model->load_data_from_property_tree( tdmap_running_configuration_ptree );
  ui->qtree_view_tdmap_running_configuration->update();

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
  boost::property_tree::ptree *tdmap_running_configuration_ptree = tdmap_running_configuration_model->save_data_into_property_tree();

  boost::property_tree::ptree *config = new boost::property_tree::ptree();

  config->add_child("project_setup_image_fields_ptree", *project_setup_image_fields_ptree);
  config->add_child("project_setup_crystalographic_fields_ptree", *project_setup_crystalographic_fields_ptree);
  config->add_child("tdmap_simulation_setup_ptree", *tdmap_simulation_setup_ptree);
  config->add_child("tdmap_running_configuration_ptree", *tdmap_running_configuration_ptree);

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
  image_path->set_item_delegate_type( TreeItem::_delegate_FILE );
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
  unit_cell_file_cif->set_item_delegate_type( TreeItem::_delegate_FILE );
  unit_cell_file->insertChildren( unit_cell_file_cif );

  ////////////////
  // Unit-cell file CEL
  ////////////////
  QVector<QVariant> box2_option_1_2 = {"CEL",""};
  QVector<bool> box2_option_1_2_edit = {false,true};
  boost::function<bool(std::string)> box2_function_1_2 ( boost::bind( &Image_Crystal::set_unit_cell_cel_path,_core_image_crystal, _1 ) );
  TreeItem* unit_cell_file_cel = new TreeItem ( box2_option_1_2 , box2_function_1_2, box2_option_1_2_edit );
  unit_cell_file_cel->set_item_delegate_type( TreeItem::_delegate_FILE );
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
  // 2D variation map
  ////////////////
  QVector<QVariant> box3_option_0 = {"Parameter variation map",""};
  TreeItem* _parameter_variation_map  = new TreeItem ( box3_option_0 );
  tdmap_root->insertChildren( _parameter_variation_map );

  ////////////////
  // 2D variation map - thickness
  ////////////////
  QVector<QVariant> box3_option_0_1 = {"Thickness",""};
  TreeItem* _parameter_variation_map_thickness  = new TreeItem ( box3_option_0_1 );
  _parameter_variation_map->insertChildren( _parameter_variation_map_thickness );

  ////////////////
  // 2D variation map - > thickness - > user estimation
  ////////////////
  QVector<QVariant> box3_option_0_1_1 = {"Estimated nm",""};
  QVector<bool> box3_option_0_1_1_edit = {false,true};
  boost::function<bool(double)> box3_function_0_1_1 ( boost::bind( &TDMap::set_thickness_user_estimated_nm, _core_td_map, _1 ) );
  TreeItem* _parameter_variation_map_thickness_estimated_nm  = new TreeItem ( box3_option_0_1_1, box3_function_0_1_1, box3_option_0_1_1_edit  );
  _parameter_variation_map_thickness_estimated_nm->set_item_delegate_type( TreeItem::_delegate_TEXT_ACTION );

  QVector<QVariant> box3_function_0_1_1_action_description = {"Auto range","Auto lower/upper"};
  boost::function<bool()> box3_function_0_1_1_auto ( boost::bind( &TDMap::auto_calculate_thickness_range_lower_upper_nm, _core_td_map ) );
  boost::function<bool()> box3_function_0_1_1_auto_lower_upper ( boost::bind( &TDMap::auto_calculate_thickness_lower_upper_nm, _core_td_map ) );

  std::vector<boost::function<bool()>> box3_function_0_1_1_actions;
  box3_function_0_1_1_actions.push_back(box3_function_0_1_1_auto);
  box3_function_0_1_1_actions.push_back(box3_function_0_1_1_auto_lower_upper);

  _parameter_variation_map_thickness_estimated_nm->add_toolbar( box3_function_0_1_1_action_description, box3_function_0_1_1_actions );

  _parameter_variation_map_thickness->insertChildren( _parameter_variation_map_thickness_estimated_nm );

  ////////////////
  //Thickness range -- Samples
  ////////////////
  QVector<QVariant> box3_option_1_3 = {"Samples",""};
  QVector<bool> box3_option_1_3_edit = {false,true};
  boost::function<bool(std::string)> box3_function_1_3 ( boost::bind( &TDMap::set_thickness_range_number_samples, _core_td_map, _1 ) );
  TreeItem* thickness_range_number_samples = new TreeItem ( box3_option_1_3 , box3_function_1_3, box3_option_1_3_edit );
  _parameter_variation_map_thickness->insertChildren( thickness_range_number_samples );

  ////////////////
  // Thickness range
  ////////////////
  QVector<QVariant> box3_option_1 = {"Thickness range",""};
  TreeItem* thickness_range  = new TreeItem ( box3_option_1 );
  _parameter_variation_map_thickness->insertChildren( thickness_range );

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
  // 2D variation map - defocus
  ////////////////
  QVector<QVariant> box3_option_0_2 = {"Defocus",""};
  TreeItem* _parameter_variation_map_defocous  = new TreeItem ( box3_option_0_2 );
  _parameter_variation_map->insertChildren( _parameter_variation_map_defocous );

  ////////////////
  // 2D variation map - > defocus - > user estimation
  ////////////////
  QVector<QVariant> box3_option_0_2_1 = {"Estimated nm",""};
  QVector<bool> box3_option_0_2_1_edit = {false,true};
  boost::function<bool(double)> box3_function_0_2_1 ( boost::bind( &TDMap::set_defocus_user_estimated_nm, _core_td_map, _1 ) );
  TreeItem* _parameter_variation_map_defocus_estimated_nm  = new TreeItem ( box3_option_0_2_1, box3_function_0_2_1, box3_option_0_2_1_edit  );
  _parameter_variation_map_defocous->insertChildren( _parameter_variation_map_defocus_estimated_nm );

  ////////////////
  //Defocus range -- Samples
  ////////////////
  QVector<QVariant> box3_option_2_3 = {"Samples",""};
  QVector<bool> box3_option_2_3_edit = {false,true};
  boost::function<bool(std::string)> box3_function_2_3 ( boost::bind( &TDMap::set_defocus_range_number_samples, _core_td_map, _1 ) );
  TreeItem* defocus_range_number_samples = new TreeItem ( box3_option_2_3 , box3_function_2_3, box3_option_2_3_edit );
  _parameter_variation_map_defocous->insertChildren( defocus_range_number_samples );

  ////////////////
  // Defocus range
  ////////////////
  QVector<QVariant> box3_option_2 = {"Defocus range",""};
  TreeItem* defocus_range  = new TreeItem ( box3_option_2 );
  _parameter_variation_map_defocous->insertChildren( defocus_range );

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
  // Incident electron beam
  ////////////////
  QVector<QVariant> box3_option_3 = {"Incident electron beam",""};
  TreeItem* incident_electron_beam  = new TreeItem ( box3_option_3 );
  _parameter_variation_map->insertChildren( incident_electron_beam );

  ////////////////
  // Incident electron beam -- Accelaration voltage (kV)
  ////////////////
  QVector<QVariant> box3_option_3_1 = {"Accelaration voltage (kV)",""};
  QVector<bool> box3_option_3_1_edit = {false,true};
  boost::function<bool(std::string)> box3_function_3_1 ( boost::bind( &TDMap::set_accelaration_voltage_kv, _core_td_map, _1 ) );
  TreeItem* accelaration_voltage_kv = new TreeItem ( box3_option_3_1 , box3_function_3_1, box3_option_3_1_edit );
  incident_electron_beam->insertChildren( accelaration_voltage_kv );

  ////////////////
  // Simulation Refinement
  ////////////////
  QVector<QVariant> box3_option_4 = {"Refinement",""};
  QVector<bool> box3_option_4_edit = {false,true};

  boost::function<int(void)> box3_function_4_getter ( boost::bind( &TDMap::get_refinement_definition_method, _core_td_map ) );
  boost::function<bool(int)> box3_function_4_setter ( boost::bind( &TDMap::set_refinement_definition_method, _core_td_map, _1 ) );

  TreeItem* _simulation_refinement  = new TreeItem ( box3_option_4, box3_function_4_setter, box3_function_4_getter, box3_option_4_edit );
  // load the preset data from core constuctor
  _simulation_refinement->load_data_from_getter();
  QVector<QVariant> box3_option_4_drop = {"No refinement","Corrected","Non-Corrected", "User defined"};

  QVector<QVariant> box3_option_4_drop_enum( { TDMap::RefinementPreset::NO_REFINEMENT, TDMap::RefinementPreset::MICROSCOPE_CORRECTED, TDMap::RefinementPreset::MICROSCOPE_NON_CORRECTED, TDMap::RefinementPreset::USER_DEFINED_PRESET } );

  _simulation_refinement->set_item_delegate_type( TreeItem::_delegate_DROP );
  _simulation_refinement->set_dropdown_options( 1, box3_option_4_drop, box3_option_4_drop_enum  );

  tdmap_root->insertChildren( _simulation_refinement );

  ////////////////
  //Simulation Refinement -- Aberration parameters
  ////////////////
  QVector<QVariant> box3_option_4_1 = {"Aberration parameters",""};
  TreeItem* _aberration_parameters = new TreeItem ( box3_option_4_1 );
  _simulation_refinement->insertChildren( _aberration_parameters );

  ////////////////
  //Simulation Refinement -- envelope parameters
  ////////////////
  QVector<QVariant> box3_option_4_2 = {"Envelope parameters",""};
  TreeItem* _envelope_parameters = new TreeItem ( box3_option_4_2 );
  _simulation_refinement->insertChildren( _envelope_parameters );

  ////////////////
  //Simulation Refinement -- MTF
  ////////////////
  QVector<QVariant> box3_option_4_3 = {"MTF",""};
  QVector<bool> box3_option_4_3_edit = {false,true};
  boost::function<bool(std::string)> box3_function_4_3 ( boost::bind( &TDMap::set_mtf_filename,_core_td_map, _1 ) );
  TreeItem* _mtf_parameters = new TreeItem ( box3_option_4_3 , box3_function_4_3, box3_option_4_3_edit );
  _mtf_parameters->set_item_delegate_type( TreeItem::_delegate_FILE );
  _simulation_refinement->insertChildren( _mtf_parameters );

  ////////////////
  // Image Correlation
  ////////////////
  QVector<QVariant> box3_option_5 = {"Image correlation",""};
  TreeItem* image_correlation  = new TreeItem ( box3_option_5 );
  tdmap_root->insertChildren( image_correlation );

  ////////////////
  // Image Correlation -- Hysteresis Thresholding
  ////////////////
  //: , non-normalized correlation and sum-absolute-difference

  QVector<QVariant> box3_option_5_1 = {"Match method",""};
  QVector<bool> box3_option_5_1_edit = {false,true};

  boost::function<int(void)> box3_function_5_1_getter ( boost::bind( &TDMap::get_image_correlation_matching_method, _core_td_map ) );
  boost::function<bool(int)> box3_function_5_1_setter ( boost::bind( &TDMap::set_image_correlation_matching_method, _core_td_map, _1 ) );
  TreeItem* image_correlation_matching_method = new TreeItem ( box3_option_5_1 , box3_function_5_1_setter, box3_function_5_1_getter,  box3_option_5_1_edit );
  // load the preset data from core constuctor
  image_correlation_matching_method->load_data_from_getter();

  QVector<QVariant> box3_option_5_1_drop = {"Normalized squared difference","Normalized cross correlation","Normalized correlation coefficient"};
  QVector<QVariant> box3_option_5_1_drop_enum( { CV_TM_SQDIFF_NORMED, CV_TM_CCORR_NORMED, CV_TM_CCOEFF_NORMED} );

  image_correlation_matching_method->set_item_delegate_type( TreeItem::_delegate_DROP );
  image_correlation_matching_method->set_dropdown_options( 1, box3_option_5_1_drop, box3_option_5_1_drop_enum  );

  image_correlation->insertChildren( image_correlation_matching_method );

  tdmap_simulation_setup_model = new TreeModel( tdmap_root );

  ui->qtree_view_tdmap_simulation_setup->setModel( tdmap_simulation_setup_model );
  ui->qtree_view_tdmap_simulation_setup->setItemDelegate( _load_file_delegate );
  //start editing after one click
  ui->qtree_view_tdmap_simulation_setup->setEditTriggers(QAbstractItemView::AllEditTriggers);

  ui->qtree_view_tdmap_simulation_setup->expandAll();
  for (int column = 0; column < tdmap_simulation_setup_model->columnCount(); ++column){
    ui->qtree_view_tdmap_simulation_setup->resizeColumnToContents(column);
  }

  ui->tdmap_table->set_tdmap( _core_td_map );

  // any change on the following fields causes the grid to be reset:
  //# thick samples, thick lower bound, thick upper bound
  //# defocus samples, lower bound, upper bound,
  // more work here

  ui->tdmap_table->connect_item_changes_to_invalidate_grid( thickness_range_number_samples, 1 );
  ui->tdmap_table->connect_item_changes_to_invalidate_grid( thickness_range_lower_bound, 1 );
  ui->tdmap_table->connect_item_changes_to_invalidate_grid( thickness_range_upper_bound, 1 );
  ui->tdmap_table->connect_item_changes_to_invalidate_grid( defocus_range_number_samples, 1 );
  ui->tdmap_table->connect_item_changes_to_invalidate_grid( defocus_range_upper_bound, 1 );
  ui->tdmap_table->connect_item_changes_to_invalidate_grid( defocus_range_lower_bound, 1 );

  ui->tdmap_table->connect_thickness_range_number_samples_changes(  thickness_range_number_samples, 1 );
  ui->tdmap_table->connect_defocus_range_number_samples_changes(  defocus_range_number_samples, 1 );

  /*************************
   * Running configuration
   *************************/

  QVector<QVariant> running_header = {"Status","Step"};

  TreeItem* running_configuration_root = new TreeItem ( running_header );

  ////////////////
  // Log level
  ////////////////

  QVector<QVariant> box4_option_0 = {"", "Log level"};
  TreeItem* _log_level  = new TreeItem ( box4_option_0 );
  running_configuration_root->insertChildren( _log_level );

  QVector<QVariant> box4_option_1 = {"", ""};
  QVector<bool> box4_option_1_edit = {false,true};
  boost::function<int()> box4_function_1_getter ( boost::bind( &TDMap::get_log_level, _core_td_map ) );
  boost::function<bool(int)> box4_function_1_setter ( boost::bind( &TDMap::set_log_level, _core_td_map, _1 ) );

  TreeItem* _log_level_setter  = new TreeItem ( box4_option_1, box4_function_1_setter, box4_function_1_getter, box4_option_1_edit );
  // load the preset data from core constuctor
  _log_level_setter->load_data_from_getter();

  QVector<QVariant> box4_option_1_drop = {"Full log","Debug mode","Silent mode", "User defined"};
  QVector<QVariant> box4_option_1_drop_enum( { TDMap::ExecLogMode::FULL_LOG, TDMap::ExecLogMode::DEBUG_MODE, TDMap::ExecLogMode::SILENT_MODE, TDMap::ExecLogMode::USER_DEFINED_LOG_MODE } );

  _log_level_setter->set_item_delegate_type( TreeItem::_delegate_DROP );
  _log_level_setter->set_dropdown_options( 1, box4_option_1_drop, box4_option_1_drop_enum  );
  _log_level->insertChildren( _log_level_setter );


  /*
   * CELSLC
   * */
  QVector<QVariant> box4_data_2 = {"",""};
  QVector<QVariant> box4_legend_2 = {"","Multislice phase granting" };
  boost::function<bool()> box4_option_2_check_getter ( boost::bind( &TDMap::get_run_celslc_switch, _core_td_map  ) );
  boost::function<bool(bool)> box4_option_2_check_setter ( boost::bind( &TDMap::set_run_celslc_switch, _core_td_map, _1 ) );
  QVector<bool> box4_option_2_edit = {false,true};
  TreeItem* _multislice_phase_granting  = new TreeItem ( box4_data_2 ,box4_option_2_check_setter, box4_option_2_check_getter, box4_legend_2, box4_option_2_edit );
  _multislice_phase_granting->set_item_delegate_type( TreeItem::_delegate_CHECK );
  running_configuration_root->insertChildren( _multislice_phase_granting );
  // load the preset data from core constuctor
  _multislice_phase_granting->load_data_from_getter();

  QVector<QVariant> box4_option_2_0 = {"", "Output"};
  TreeItem* _multislice_phase_granting_output_legend   = new TreeItem ( box4_option_2_0 );
  _multislice_phase_granting->insertChildren( _multislice_phase_granting_output_legend );

  QVector<QVariant> box4_option_2_1 = {"", ""};
  QVector<bool> box4_option_2_1_edit = {false,true};
  _multislice_phase_granting_output  = new TreeItem ( box4_option_2_1, box4_option_2_1_edit );
  _multislice_phase_granting_output->set_fp_data_data_appender_col_pos( 1 );
  _multislice_phase_granting_output->set_flag_fp_data_appender_string( true );
  _multislice_phase_granting_output->set_item_delegate_type( TreeItem::_delegate_TEXT_BROWSER );
  _multislice_phase_granting_output_legend->insertChildren( _multislice_phase_granting_output );

  QVector<QVariant> box4_option_2_2 = {"","Temporary files"};
  TreeItem* _multislice_phase_granting_temporary_files  = new TreeItem ( box4_option_2_2 );
  _multislice_phase_granting->insertChildren( _multislice_phase_granting_temporary_files );

  /*
   * MSA
   * */
  QVector<QVariant> box4_data_3 = {"",""};
  QVector<QVariant> box4_legend_3 = {"","Electron diffraction patterns" };
  boost::function<bool(void)> box4_option_3_check_getter ( boost::bind( &TDMap::get_run_msa_switch, _core_td_map ) );
  boost::function<bool(bool)> box4_option_3_check_setter ( boost::bind( &TDMap::set_run_msa_switch, _core_td_map, _1 ) );
  QVector<bool> box4_option_3_edit = {false,true};
  TreeItem* _electron_diffraction_patterns  = new TreeItem ( box4_data_3 ,box4_option_3_check_setter, box4_option_3_check_getter, box4_legend_3, box4_option_3_edit );
  // load the preset data from core constuctor
  _electron_diffraction_patterns->load_data_from_getter();
  _electron_diffraction_patterns->set_item_delegate_type( TreeItem::_delegate_CHECK );
  running_configuration_root->insertChildren( _electron_diffraction_patterns );

  QVector<QVariant> box4_option_3_0 = {"", "Output"};
  TreeItem* _electron_diffraction_patterns_output_legend   = new TreeItem ( box4_option_3_0 );
  _electron_diffraction_patterns->insertChildren( _electron_diffraction_patterns_output_legend );

  QVector<QVariant> box4_option_3_1 = {"", ""};
  QVector<bool> box4_option_3_1_edit = {false,true};
  _electron_diffraction_patterns_output  = new TreeItem ( box4_option_3_1, box4_option_3_1_edit );
  _electron_diffraction_patterns_output->set_fp_data_data_appender_col_pos( 1 );
  _electron_diffraction_patterns_output->set_flag_fp_data_appender_string( true );
  _electron_diffraction_patterns_output->set_item_delegate_type( TreeItem::_delegate_TEXT_BROWSER );
  _electron_diffraction_patterns_output_legend->insertChildren( _electron_diffraction_patterns_output );

  QVector<QVariant> box4_option_3_2 = {"","Temporary files"};
  TreeItem* _electron_diffraction_patterns_temporary_files  = new TreeItem ( box4_option_3_2 );
  _electron_diffraction_patterns->insertChildren( _electron_diffraction_patterns_temporary_files );

  /*
   * WAVIMG
   * */

  QVector<QVariant> box4_data_4 = {"",""};
  QVector<QVariant> box4_legend_4 = {"","Image intensity distribuitions"};
  boost::function<bool(void)> box4_option_4_check_getter ( boost::bind( &TDMap::get_run_wavimg_switch, _core_td_map ) );
  boost::function<bool(bool)> box4_option_4_check_setter ( boost::bind( &TDMap::set_run_wavimg_switch, _core_td_map, _1 ) );
  QVector<bool> box4_option_4_edit = {false,true};
  TreeItem* _image_intensity_distribuitions  = new TreeItem ( box4_data_4 ,box4_option_4_check_setter, box4_option_4_check_getter, box4_legend_4, box4_option_4_edit );
  _image_intensity_distribuitions->set_item_delegate_type( TreeItem::_delegate_CHECK );
  running_configuration_root->insertChildren( _image_intensity_distribuitions );
  // load the preset data from core constuctor
  _image_intensity_distribuitions->load_data_from_getter();

  QVector<QVariant> box4_option_4_0 = {"","Output"};
  TreeItem* _image_intensity_distribuitions_output_legend   = new TreeItem ( box4_option_4_0 );
  _image_intensity_distribuitions->insertChildren( _image_intensity_distribuitions_output_legend );

  QVector<QVariant> box4_option_4_1 = {"", ""};
  QVector<bool> box4_option_4_1_edit = {false,true};
  _image_intensity_distribuitions_output  = new TreeItem ( box4_option_4_1, box4_option_4_1_edit );
  _image_intensity_distribuitions_output->set_fp_data_data_appender_col_pos( 1 );
  _image_intensity_distribuitions_output->set_flag_fp_data_appender_string( true );
  _image_intensity_distribuitions_output->set_item_delegate_type( TreeItem::_delegate_TEXT_BROWSER );
  _image_intensity_distribuitions_output_legend->insertChildren( _image_intensity_distribuitions_output );

  QVector<QVariant> box4_option_4_2 = {"","Temporary files"};
  TreeItem* _image_intensity_distribuitions_temporary_files  = new TreeItem ( box4_option_4_2 );
  _image_intensity_distribuitions->insertChildren( _image_intensity_distribuitions_temporary_files );

  /*
   * SIMGRID
   * */
  QVector<QVariant> box4_data_5 = {"",""};
  QVector<QVariant> box4_legend_5 = {"","Image correlation"};
  boost::function<bool(void)> box4_option_5_check_getter ( boost::bind( &TDMap::get_run_simgrid_switch, _core_td_map ) );
  boost::function<bool(bool)> box4_option_5_check_setter ( boost::bind( &TDMap::set_run_simgrid_switch, _core_td_map, _1 ) );
  QVector<bool> box4_option_5_edit = {false,true};
  TreeItem* _image_correlation  = new TreeItem ( box4_data_5 ,box4_option_5_check_setter, box4_option_5_check_getter, box4_legend_5, box4_option_5_edit );
  _image_correlation->set_item_delegate_type( TreeItem::_delegate_CHECK );
  running_configuration_root->insertChildren( _image_correlation );
  // load the preset data from core constuctor
  _image_correlation->load_data_from_getter();

  QVector<QVariant> box4_option_5_0 = {"","Output"};
  TreeItem* _image_correlation_output_legend  = new TreeItem ( box4_option_5_0 );
  _image_correlation->insertChildren( _image_correlation_output_legend );

  QVector<QVariant> box4_option_5_1 = {"", ""};
  QVector<bool> box4_option_5_1_edit = {false,true};
  _image_correlation_output  = new TreeItem ( box4_option_5_1, box4_option_5_1_edit );
  _image_correlation_output->set_fp_data_data_appender_col_pos( 1 );
  _image_correlation_output->set_flag_fp_data_appender_string( true );
  _image_correlation_output->set_item_delegate_type( TreeItem::_delegate_TEXT_BROWSER );
  _image_correlation_output_legend->insertChildren( _image_correlation_output );

  tdmap_running_configuration_model = new TreeModel( running_configuration_root );
  ui->qtree_view_tdmap_running_configuration->setModel( tdmap_running_configuration_model );
  ui->qtree_view_tdmap_running_configuration->setItemDelegate( _load_file_delegate );
  //start editing after one click
  ui->qtree_view_tdmap_running_configuration->setEditTriggers(QAbstractItemView::AllEditTriggers);
  for (int column = 0; column < tdmap_simulation_setup_model->columnCount(); ++column){
    ui->qtree_view_tdmap_running_configuration->resizeColumnToContents(column);
  }
}

bool MainWindow::set_dr_probe_path( QString path ){
  _dr_probe_bin_path = path;
  return true;
}
