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
  ui->super_cell_splitter->setStretchFactor(0,3);
  ui->super_cell_splitter->setStretchFactor(1,7);
  if (_flag_im2model_logger) {
    im2model_logger->logEvent(ApplicationLog::normal, "Creating actions.");
  }
  createActions();

  if (_flag_im2model_logger) {
    im2model_logger->logEvent(ApplicationLog::normal, "Creating progress bar.");
  }
  createProgressBar();

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
      im2model_logger->logEvent( ApplicationLog::critical, "Preferences are not setted. Trying to resolve." );
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

    _core_td_map = new TDMap(
        _sim_tdmap_celslc_ostream_buffer,
        _sim_tdmap_msa_ostream_buffer,
        _sim_tdmap_wavimg_ostream_buffer,
        _sim_tdmap_simgrid_ostream_buffer );

    celslc_step_group_options = new group_options("celslc_step");
    msa_step_group_options = new group_options("msa_step");
    wavimg_step_group_options = new group_options("wavimg_step");
    simgrid_step_group_options = new group_options("simgrid_step");
    super_cell_target_step_group_options = new group_options("super_cell_target_step");

    // dr probe pipeline dependencies
    msa_step_group_options->listen_group_update_required( celslc_step_group_options );
    wavimg_step_group_options->listen_group_update_required( msa_step_group_options );
    // tdmap dependency
    simgrid_step_group_options->listen_group_update_required( wavimg_step_group_options );

    _core_td_map->set_group_options( celslc_step_group_options, msa_step_group_options, wavimg_step_group_options, simgrid_step_group_options );

    if (_flag_im2model_logger) {
      im2model_logger->logEvent(ApplicationLog::critical, "Trying to set application logger for TDMap and SuperCell.");
      _core_td_map->set_application_logger(im2model_logger);
    }

    bool status = true;
    //  status &= _core_td_map->set_dr_probe_bin_path( _dr_probe_bin_path.toStdString() );
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
      setCurrentFile(QString());

      create_box_options();

      /* TDMap simulation thread */
      _sim_tdmap_thread = new QThread( this );
      sim_tdmap_worker = new GuiSimOutUpdater( _core_td_map  );

      sim_tdmap_worker->moveToThread( _sim_tdmap_thread );

      // will only start thread when needed
      connect(sim_tdmap_worker, SIGNAL(TDMap_request()), _sim_tdmap_thread, SLOT(start()));
      connect(_sim_tdmap_thread, SIGNAL(started()), sim_tdmap_worker, SLOT(newTDMapSim()));
      connect(sim_tdmap_worker, SIGNAL(TDMap_sucess()), this, SLOT(update_from_TDMap_sucess( )));
      connect(sim_tdmap_worker, SIGNAL(TDMap_sucess_no_correlation()), this, SLOT(update_from_TDMap_sucess( )));
      connect(sim_tdmap_worker, SIGNAL(TDMap_failure()), this, SLOT(update_from_TDMap_failure()));
      // will quit thread after work done
      connect(sim_tdmap_worker, SIGNAL(TDMap_finished()), _sim_tdmap_thread, SLOT(quit()), Qt::DirectConnection);

      /* Super-Cell Edge Detection  thread */
      _sim_super_cell_thread = new QThread( this );
      sim_super_cell_worker = new GuiSimOutUpdater( _core_td_map );
      sim_super_cell_worker->moveToThread( _sim_super_cell_thread );

      // will only start thread when needed
      connect( sim_super_cell_worker, SIGNAL(SuperCell_edge_request()), _sim_super_cell_thread, SLOT(start()));
      connect(_sim_super_cell_thread, SIGNAL(started()), sim_super_cell_worker, SLOT(newSuperCellEdge()));

      connect(sim_super_cell_worker, SIGNAL(SuperCell_edge_sucess()), this, SLOT(update_from_SuperCell_edge_sucess()));
      connect(sim_super_cell_worker, SIGNAL(SuperCell_edge_failure()), this, SLOT(update_from_SuperCell_edge_failure()));
      // will quit thread after work done
      connect(sim_super_cell_worker, SIGNAL(SuperCell_edge_finished()), _sim_super_cell_thread, SLOT(quit()), Qt::DirectConnection);

      connect( ui->tdmap_table, SIGNAL(tdmap_best_match( int, int )), this, SLOT( update_tdmap_best_match(int,int)) );
      connect(ui->tdmap_table, SIGNAL(cellClicked(int , int )), this, SLOT( update_tdmap_current_selection(int,int)) );

      connect(this, SIGNAL(experimental_image_filename_changed()), this, SLOT(update_full_experimental_image()));
      connect(this, SIGNAL(simulated_grid_changed( )), this, SLOT(update_simgrid_frame( )));

      connect(this, SIGNAL(super_cell_target_region_changed()), this, SLOT(update_super_cell_target_region()));

      connect( _core_td_map, SIGNAL(TDMap_started_celslc( )), this, SLOT(update_tdmap_celslc_started( ) ) );
      connect(_core_td_map, SIGNAL(TDMap_started_celslc()), this, SLOT(update_tdmap_sim_ostream_celslc()));
      connect( _core_td_map, SIGNAL(TDMap_at_celslc_step( int )), this, SLOT(update_tdmap_celslc_step( int ) ) );
      connect( _core_td_map, SIGNAL(TDMap_ended_celslc( bool )), this, SLOT(update_tdmap_celslc_ended( bool ) ) );

      connect( _core_td_map, SIGNAL(TDMap_started_msa( )), this, SLOT(update_tdmap_msa_started( ) ) );
      connect(_core_td_map, SIGNAL(TDMap_started_msa()), this, SLOT(update_tdmap_sim_ostream_msa()));
      connect( _core_td_map, SIGNAL(TDMap_ended_msa( bool )), this, SLOT(update_tdmap_msa_ended( bool ) ) );

      connect( _core_td_map, SIGNAL(TDMap_started_wavimg( )), this, SLOT(update_tdmap_wavimg_started( ) ) );
      connect(_core_td_map, SIGNAL(TDMap_started_wavimg()), this, SLOT(update_tdmap_sim_ostream_wavimg()));
      connect( _core_td_map, SIGNAL(TDMap_ended_wavimg( bool )), this, SLOT(update_tdmap_wavimg_ended( bool ) ) );

      connect( _core_td_map, SIGNAL(TDMap_started_simgrid( )), this, SLOT(update_tdmap_simgrid_started( ) ) );
      connect(_core_td_map, SIGNAL(TDMap_started_simgrid()), this, SLOT(update_tdmap_sim_ostream_simgrid()));
      connect( _core_td_map, SIGNAL(TDMap_ended_simgrid( bool )), this, SLOT(update_tdmap_simgrid_ended( bool ) ) );
      connect( _core_td_map, SIGNAL(TDMap_no_simgrid( bool )), this, SLOT(update_tdmap_no_simgrid_ended( bool ) ) );


      if( _flag_im2model_logger ){
        im2model_logger->logEvent( ApplicationLog::notification, "Finished initializing App." );
      }
    }
  }
}

void MainWindow::setApplicationVersion( std::string app_version ){
  application_version = app_version;
  _flag_application_version = true;
}

void MainWindow::update_tdmap_celslc_started( ){
  updateProgressBar(0,0,4);
  ui->statusBar->showMessage(tr("Started multislice step"), 2000);
}

void MainWindow::update_tdmap_celslc_ended( bool result ){
  // reset pipe
  _sim_tdmap_celslc_ostream_buffer.pipe( boost::process::pipe() );
  if( result ){
    ui->statusBar->showMessage(tr("Sucessfully ended multislice step"), 2000);
  }
  else{
    ui->statusBar->showMessage(tr("Error while running multislice step") );
  }
}

void MainWindow::update_tdmap_msa_started( ){
  updateProgressBar(0,1,4);
  ui->statusBar->showMessage(tr("Started calculating electron diffraction patterns"), 2000);
}

void MainWindow::update_tdmap_msa_ended( bool result ){
  // reset pipe
  _sim_tdmap_msa_ostream_buffer.pipe( boost::process::pipe() );
  if( result ){
    updateProgressBar(0,2,4);
    ui->statusBar->showMessage(tr("Sucessfully ended calculating the electron diffraction patterns"), 2000);
  }
  else{
    updateProgressBar(0,2,4, true);
    ui->statusBar->showMessage(tr("Error while calculating the electron diffraction patterns") );
  }
}

void MainWindow::update_tdmap_wavimg_started( ){
  updateProgressBar(0,2,4);
  ui->statusBar->showMessage(tr("Started calculating image intensity distribuitions"), 2000);
}

void MainWindow::update_tdmap_wavimg_ended( bool result ){
  // reset pipe
  _sim_tdmap_wavimg_ostream_buffer.pipe( boost::process::pipe() );
  if( result ){
    updateProgressBar(0,3,4);
    ui->statusBar->showMessage(tr("Sucessfully ended calculating the image intensity distribuitions"), 2000);
  }
  else{
    updateProgressBar(0,3,4, true);
    ui->statusBar->showMessage(tr("Error while calculating the image intensity distribuitions") );
  }
}

void MainWindow::update_tdmap_simgrid_started( ){
  // reset pipe
  _sim_tdmap_simgrid_ostream_buffer.pipe( boost::process::pipe() );
  updateProgressBar(0,3,4);
  ui->statusBar->showMessage(tr("Started image correlation step"), 2000);
}

void MainWindow::update_tdmap_simgrid_ended( bool result ){
  if( result ){
    updateProgressBar(0,4,4);
    ui->statusBar->showMessage(tr("Sucessfully ended image correlation step"), 2000);
  }
  else{
    updateProgressBar(0,4,4, true);
    ui->statusBar->showMessage(tr("Error while running image correlation step") );
  }
}

void MainWindow::update_tdmap_no_simgrid_ended( bool result ){
  if( result ){
    updateProgressBar(0,4,4);
    ui->statusBar->showMessage(tr("Sucessfully ended creating TDMap without image correlation step"), 2000);
  }
  else{
    updateProgressBar(0,4,4, true);
    ui->statusBar->showMessage(tr("Error while creating TDMap without image correlation step") );
  }
}

void MainWindow::update_tdmap_celslc_step( int at_step ){

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
        tr("The saved prefences file is incomplete.\n""To use Im2Model all preferences vars must be setted.\n"
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
  if(_core_td_map->get_flag_simulated_images_grid()){
    cv::Mat _simulated_image = _core_td_map->get_simulated_image_in_grid(x,y);
    double _simulated_image_match = 0.0f;
    const bool correlation_active = _core_td_map->get_run_simgrid_switch();
    QStandardItem *match_item;
    if( correlation_active ){
      _simulated_image_match = _core_td_map->get_simulated_image_match_in_grid(x,y);
      match_item = new QStandardItem( QString::number( _simulated_image_match ) );
    }
    else{
      match_item = new QStandardItem(tr("N/A"));
    }
    const double _simulated_image_thickness = _core_td_map->get_simulated_image_thickness_nm_in_grid(x,y);
    const double _simulated_image_defocus = _core_td_map->get_simulated_image_defocus_in_grid(x,y);

    QStandardItemModel* model = new QStandardItemModel(3, 2,this);
    model->setHeaderData(0, Qt::Horizontal, tr("Parameter"));
    model->setHeaderData(1, Qt::Horizontal, tr("Value"));
    QStandardItem *label_match_item = new QStandardItem(tr("Match %"));
    QStandardItem *label_defocus_item = new QStandardItem(tr("Defocus"));
    QStandardItem *label_thickness_item = new QStandardItem(tr("Thickness"));
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
  if(_core_td_map->get_flag_simulated_images_grid()){
    cv::Mat _simulated_image = _core_td_map->get_simulated_image_in_grid(x,y);
    double _simulated_image_match = 0.0f;
    const bool correlation_active = _core_td_map->get_run_simgrid_switch();
    QStandardItem *match_item;
    if( correlation_active ){
      _simulated_image_match = _core_td_map->get_simulated_image_match_in_grid(x,y);
      match_item = new QStandardItem( QString::number( _simulated_image_match ) );
    }
    else{
      match_item = new QStandardItem(tr("N/A"));
    }

    const double _simulated_image_thickness = _core_td_map->get_simulated_image_thickness_nm_in_grid(x,y);
    const double _simulated_image_defocus = _core_td_map->get_simulated_image_defocus_in_grid(x,y);

    QStandardItemModel* model = new QStandardItemModel(3, 2,this);

    model->setHeaderData(0, Qt::Horizontal, tr("Parameter"));
    model->setHeaderData(1, Qt::Horizontal, tr("Value"));
    QStandardItem *label_match_item = new QStandardItem(tr("Match %"));
    QStandardItem *label_defocus_item = new QStandardItem(tr("Defocus"));
    QStandardItem *label_thickness_item = new QStandardItem(tr("Thickness"));
    QStandardItem *thickness_item = new QStandardItem( QString::number( _simulated_image_thickness ) );
    QStandardItem *defocus_item = new QStandardItem( QString::number( _simulated_image_defocus  ) );
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
    sim_super_cell_worker->abort();
    _sim_tdmap_thread->wait();
    _sim_super_cell_thread->wait();
    ////qDebug()<<"Deleting thread and worker in Thread "<<this->QObject::thread()->currentThreadId();
    delete _sim_tdmap_thread;
    delete _sim_super_cell_thread;
    delete sim_tdmap_worker;
    delete sim_super_cell_worker;
  }
  /* DELETE UI */
  delete ui;
}

bool MainWindow::update_qline_image_path( std::string fileName ){
  bool status = false;
  const bool _td_map_load_ok = _core_td_map->set_exp_image_properties_full_image( fileName );
  if( _td_map_load_ok ){
    emit experimental_image_filename_changed();
    status = true;
  }
  return status;
}

void MainWindow::update_simgrid_frame(){
  std::vector< std::vector<cv::Mat> > _simulated_images_grid = _core_td_map->get_simulated_images_grid();
  this->ui->tdmap_table->set_simulated_images_grid( _simulated_images_grid );
}

void MainWindow::update_super_cell_target_region(){
  /*  cv::Mat target_region = _core_super_cell->get_full_image();
      this->ui->qgraphics_super_cell_edge_detection->setImage( target_region );
      */
  this->ui->qgraphics_super_cell_edge_detection->cleanRenderAreas();
  update_super_cell_target_region_shapes();
  this->ui->qgraphics_super_cell_edge_detection->show();
}

void MainWindow::update_super_cell_target_region_shapes(){
  this->ui->qgraphics_super_cell_edge_detection->cleanRenderAreas();
  // boundary rect
  if( _core_td_map->get_exp_image_bounds_flag_roi_boundary_rect() ){
    cv::Rect _rect_boundary_polygon = _core_td_map->get_exp_image_bounds_roi_boundary_rect();
    ui->qgraphics_super_cell_edge_detection->addShapeRect( _rect_boundary_polygon, 10 , tr("Target region bounding rectangle") );
  }
  // boundary rect  with margin
  if( _core_td_map->get_exp_image_bounds_flag_roi_boundary_rect_w_margin() ){
    cv::Rect _rect_w_margin_boundary_polygon = _core_td_map->get_exp_image_bounds_roi_boundary_rect_w_margin();
    ui->qgraphics_super_cell_edge_detection->addShapeRect( _rect_w_margin_boundary_polygon, 10, cv::Vec3b(255,0,255), tr("Target region with margin bounding rectangle") );
  }
  // experimental image boundary polygon
  if( _core_td_map->get_exp_image_bounds_flag_full_boundary_polygon() ){
    std::vector<cv::Point2i> boundary_polygon = _core_td_map->get_exp_image_bounds_full_boundary_polygon();
    ui->qgraphics_super_cell_edge_detection->addShapePolygon( boundary_polygon, cv::Point2i( 0,0 ), 10, cv::Vec3b(0,255,0) , tr("Target region boundary") );
  }
  // experimental image boundary polygon w margin
  if( _core_td_map->get_exp_image_bounds_flag_full_boundary_polygon_w_margin() ){
    std::vector<cv::Point2i> boundary_polygon_w_margin = _core_td_map->get_exp_image_bounds_full_boundary_polygon_w_margin();
    ui->qgraphics_super_cell_edge_detection->addShapePolygon( boundary_polygon_w_margin, cv::Point2i( 0,0 ), 10, cv::Vec3b(0,0,255) , tr("Target region with margin boundary") );
  }
  this->ui->qgraphics_super_cell_edge_detection->show();
}

void MainWindow::update_full_experimental_image(){
  if( _core_td_map->get_exp_image_properties_flag_full_image() ){
    cv::Mat full_image = _core_td_map->get_exp_image_properties_full_image();
    // update tab 1
    ui->qgraphics_full_experimental_image->setImage( full_image );
    ui->qgraphics_full_experimental_image->show();
    // update tab 3
    this->ui->qgraphics_super_cell_edge_detection->setImage( full_image );
    this->ui->qgraphics_super_cell_edge_detection->show();
    update_roi_experimental_image_frame();
  }
}

void MainWindow::update_roi_experimental_image_frame(){
  if( _core_td_map->get_exp_image_properties_flag_roi_image() ){
    cv::Mat roi_image = _core_td_map->get_exp_image_properties_roi_image();
    ui->qgraphics_roi_experimental_image->setImage( roi_image );
    ui->qgraphics_roi_experimental_image->show();
  }
}

void MainWindow::update_roi_full_experimental_image_frame(){
  if( _core_td_map->get_exp_image_properties_flag_roi_rectangle() ){
    cv::Rect _roi_rect = _core_td_map->get_exp_image_properties_roi_rectangle();
    ui->qgraphics_full_experimental_image->cleanRenderAreas();
    ui->qgraphics_full_experimental_image->addShapeRect( _roi_rect, 10 , tr("ROI boundary") );
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
    result |= super_cell_setup_model->_was_model_modified();
  }
  return result;
}

bool MainWindow::_reset_document_modified_flags(){
  bool result = false;
  if( _settings_ok ){
    result |= project_setup_image_fields_model->_reset_model_modified();
    result |= project_setup_crystalographic_fields_model->_reset_model_modified();
    result |= tdmap_simulation_setup_model->_reset_model_modified();
    result |= tdmap_running_configuration_model->_reset_model_modified();
    result |= super_cell_setup_model->_reset_model_modified();
  }
  return result;
}

void MainWindow::update_from_TDMap_sucess( ){
  ui->statusBar->showMessage(tr("Sucessfully runned TD-Map"), 2000);
  updateProgressBar(0,4,4);
  emit simulated_grid_changed( );
}

void MainWindow::update_from_TDMap_failure(){
  ui->statusBar->showMessage(tr("Error while running TD-Map") );
  std::vector <std::string> errors = _core_td_map->get_test_run_config_errors();
  std::ostringstream os;
  for( int pos = 0; pos < errors.size(); pos++ ){
    os << errors.at(pos) << "\n";
  }
  QMessageBox messageBox;
  QFont font;
  font.setBold(false);
  messageBox.setFont(font);
  messageBox.critical(0,"Error",QString::fromStdString( os.str() ));
}

void MainWindow::update_from_SuperCell_edge_sucess(){
  ui->statusBar->showMessage(tr("Sucessfully runned edge detection"), 2000);
  emit super_cell_target_region_changed();
}

void MainWindow::update_from_SuperCell_edge_failure(){
  ui->statusBar->showMessage(tr("Error while running edge detection") );
}


void MainWindow::clear_tdmap_sim_ostream_containers(){

  QModelIndex celslc_index = tdmap_running_configuration_model->index(1,0);
  QModelIndex celslc_out_legend_index = tdmap_running_configuration_model->index(0,0,celslc_index);
  QModelIndex celslc_out_index = project_setup_crystalographic_fields_model->index(0,1,celslc_out_legend_index);
  tdmap_running_configuration_model->clearData( celslc_out_index );

  QModelIndex msa_index = tdmap_running_configuration_model->index(2,0);
  QModelIndex msa_out_legend_index = tdmap_running_configuration_model->index(0,0,msa_index);
  QModelIndex msa_out_index = project_setup_crystalographic_fields_model->index(0,1,msa_out_legend_index);
  tdmap_running_configuration_model->clearData( msa_out_index );

  QModelIndex wavimg_index = tdmap_running_configuration_model->index(3,0);
  QModelIndex wavimg_out_legend_index = tdmap_running_configuration_model->index(0,0,wavimg_index);
  QModelIndex wavimg_out_index = project_setup_crystalographic_fields_model->index(0,1,wavimg_out_legend_index);
  tdmap_running_configuration_model->clearData( wavimg_out_index );

  QModelIndex simgrid_index = tdmap_running_configuration_model->index(2,0);
  QModelIndex simgrid_out_legend_index = tdmap_running_configuration_model->index(0,0,simgrid_index);
  QModelIndex simgrid_out_index = project_setup_crystalographic_fields_model->index(0,1,simgrid_out_legend_index);
  tdmap_running_configuration_model->clearData( simgrid_out_index );
}

void MainWindow::update_tdmap_sim_ostream_celslc(){
  std::string line;
  if( _core_td_map->get_run_celslc_switch() ){
    QModelIndex celslc_index = tdmap_running_configuration_model->index(1,0);
    QModelIndex celslc_out_legend_index = tdmap_running_configuration_model->index(0,0,celslc_index);
    QModelIndex celslc_out_index = project_setup_crystalographic_fields_model->index(0,1,celslc_out_legend_index);

    if( _core_td_map->get_flag_celslc_io_ap_pipe_out() ){
      while(std::getline(_sim_tdmap_celslc_ostream_buffer, line)){

        QString qt_linw =  QString::fromStdString( line );
        QVariant _new_line_var = QVariant::fromValue(qt_linw + "\n");
        bool result = tdmap_running_configuration_model->appendData( celslc_out_index, _new_line_var );
        ui->qtree_view_tdmap_simulation_setup->update();
        QApplication::processEvents();
      }
    }
  }
}

void MainWindow::update_tdmap_sim_ostream_msa(){
  std::string line;
  if( _core_td_map->get_run_msa_switch() ){

    QModelIndex msa_index = tdmap_running_configuration_model->index(2,0);
    QModelIndex msa_out_legend_index = tdmap_running_configuration_model->index(0,0,msa_index);
    QModelIndex msa_out_index = project_setup_crystalographic_fields_model->index(0,1,msa_out_legend_index);

    if( _core_td_map->get_flag_msa_io_ap_pipe_out() ){
      while(std::getline(_sim_tdmap_msa_ostream_buffer, line)){
        QString qt_linw =  QString::fromStdString( line );
        QVariant _new_line_var = QVariant::fromValue(qt_linw + "\n");
        bool result = tdmap_running_configuration_model->appendData( msa_out_index, _new_line_var );
        ui->qtree_view_tdmap_simulation_setup->update();
        QApplication::processEvents();
      }
    }
  }
}

void MainWindow::update_tdmap_sim_ostream_wavimg(){
  std::string line;
  if( _core_td_map->get_run_wavimg_switch() ){
    QModelIndex wavimg_index = tdmap_running_configuration_model->index(3,0);
    QModelIndex wavimg_out_legend_index = tdmap_running_configuration_model->index(0,0,wavimg_index);
    QModelIndex wavimg_out_index = project_setup_crystalographic_fields_model->index(0,1,wavimg_out_legend_index);

    if( _core_td_map->get_flag_wavimg_io_ap_pipe_out() ){

      while(std::getline(_sim_tdmap_wavimg_ostream_buffer, line)){
        QString qt_linw =  QString::fromStdString( line );
        QVariant _new_line_var = QVariant::fromValue(qt_linw + "\n");
        bool result = tdmap_running_configuration_model->appendData( wavimg_out_index, _new_line_var );
        ui->qtree_view_tdmap_simulation_setup->update();
        QApplication::processEvents();
      }
    }
  }
}

void MainWindow::update_tdmap_sim_ostream_simgrid(){
  std::string line;

  if( _core_td_map->get_run_simgrid_switch() ){
    if( _core_td_map->get_flag_simgrid_io_ap_pipe_out() ){
      QModelIndex simgrid_index = tdmap_running_configuration_model->index(2,0);
      QModelIndex simgrid_out_legend_index = tdmap_running_configuration_model->index(0,0,simgrid_index);
      QModelIndex simgrid_out_index = project_setup_crystalographic_fields_model->index(0,1,simgrid_out_legend_index);
    }
  }
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

bool MainWindow::save(){
  bool result = false;
  if ( curFile.isEmpty() ){
    result = saveAs();
  }
  else {
    result = saveFile(curFile);
  }
  // if the saving was ok, then we are in a project
  if( result ){
    _flag_project_setted = true;
  }
  return result;
}

bool MainWindow::saveAs()
{
  bool result = false;
  QFileDialog dialog(this);
  dialog.setWindowModality(Qt::WindowModal);
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  if ( dialog.exec() == QDialog::Accepted ){
    QString folder_path = dialog.selectedFiles().first();
    result = _core_td_map->set_project_dir_path( folder_path.toStdString() );
    if( result ){
      std::string project_filename_with_path = _core_td_map->get_project_filename_with_path();
      QString qt_project_filename_with_path = QString::fromStdString( project_filename_with_path );
      result = saveFile( qt_project_filename_with_path );
    }
  }
  return result;
}


bool MainWindow::export_TDMap_full( ){
  return export_TDMap( false );
}

bool MainWindow::export_TDMap_cutted( ){
  return export_TDMap( true );
}

bool MainWindow::export_TDMap( bool cut_margin ){
  // returns false if no settings were saved
  bool result = false;
  if( _core_td_map ){
    if( _core_td_map->get_flag_raw_simulated_images_grid() ){
      QFileDialog dialog(this);
      dialog.setWindowModality(Qt::WindowModal);
      std::string preset_filename = _core_td_map->get_export_sim_grid_filename_hint();
      boost::filesystem::path base_dir_path = _core_td_map->get_project_dir_path();
      boost::filesystem::path filename( preset_filename );
      boost::filesystem::path full_path_filename = base_dir_path / preset_filename ;
      dialog.selectFile( QString::fromStdString(full_path_filename.string() ) );

      dialog.setAcceptMode(QFileDialog::AcceptSave);
      if (dialog.exec() != QDialog::Accepted){
        result = false;
      }
      else{
        QString tdmap_filename = dialog.selectedFiles().first();
        result = _core_td_map->export_sim_grid(  tdmap_filename.toStdString() , cut_margin );
        if( result ){
          ui->statusBar->showMessage(tr("TDMap correctly exported to file: " ) + tdmap_filename , 2000);
        }
      }
    }
  }
  if( !result ){
    ui->statusBar->showMessage(tr("Error while exporting TDMap") );
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
  dialog.set_dr_probe_celslc_bin( _dr_probe_celslc_bin.toStdString() );
  dialog.set_dr_probe_msa_bin( _dr_probe_msa_bin.toStdString() );
  dialog.set_dr_probe_wavimg_bin( _dr_probe_wavimg_bin.toStdString() );
  dialog.produce_settings_panel();
  dialog.exec();
  if ( dialog._is_save_preferences() ){
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
  QAction *exportTDMap_cut = tdmapMenu->addAction( tr("&Export "), this , &MainWindow::export_TDMap_cutted );
  // newAct->setShortcuts(QKeySequence::);
  newAct->setStatusTip(tr("Export the current TD Map"));
  tdmapMenu->addAction( exportTDMap_cut );
  QAction *exportTDMap_mrg = tdmapMenu->addAction( tr("&Export with super-cell margins"), this , &MainWindow::export_TDMap_full );
  // newAct->setShortcuts(QKeySequence::);
  newAct->setStatusTip(tr("Export the current TD Map with super-cell margin"));
  tdmapMenu->addAction( exportTDMap_mrg );

}

void MainWindow::createProgressBar(){
  running_progress = new QProgressBar(this);
  running_progress->setVisible(false);
  ui->statusBar->addPermanentWidget(running_progress);
}

void MainWindow::updateProgressBar( int lower_range, int current_value, int upper_range ){
  running_progress->setRange( lower_range, upper_range );
  running_progress->setValue( current_value );
  running_progress->setVisible(true);
}

void MainWindow::updateProgressBar( int lower_range, int current_value, int upper_range, bool error ){
  updateProgressBar( lower_range, current_value, upper_range);
  if( error ){
    QPalette p = palette();
    p.setColor(QPalette::Highlight, Qt::red);
    running_progress->setPalette(p);
  }
}

bool MainWindow::checkSettings(){
  bool status = true;
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
  }

  // check if the bins are not equal to ""
  _temp_flag_dr_probe_celslc_bin &= (_dr_probe_celslc_bin == QString("")) ? false : true;
  _temp_flag_dr_probe_msa_bin &= (_dr_probe_msa_bin == QString("")) ? false : true;
  _temp_flag_dr_probe_wavimg_bin &= (_dr_probe_wavimg_bin == QString("")) ? false : true;

  if (_temp_flag_dr_probe_celslc_bin) {
    boost::filesystem::path full_path_filename(_dr_probe_celslc_bin.toStdString());
    if (boost::filesystem::exists(full_path_filename)) {
      _temp_flag_dr_probe_celslc_bin = true;
    }
    else {
      _temp_flag_dr_probe_celslc_bin = false;
    }
  }
  if (_temp_flag_dr_probe_msa_bin) {
    boost::filesystem::path full_path_filename(_dr_probe_msa_bin.toStdString());
    if (boost::filesystem::exists(full_path_filename)) {
      _temp_flag_dr_probe_msa_bin = true;
    }
    else {
      _temp_flag_dr_probe_msa_bin = false;
    }
  }

  if (_temp_flag_dr_probe_wavimg_bin) {
    boost::filesystem::path full_path_filename(_dr_probe_celslc_bin.toStdString());
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
    message << "celslc path: " <<  _dr_probe_celslc_bin.toStdString() << " _flag_dr_probe_celslc_bin: " << _temp_flag_dr_probe_celslc_bin;
    im2model_logger->logEvent(ApplicationLog::notification, message.str());
    message = std::stringstream();
    message << "msa path: " <<  _dr_probe_msa_bin.toStdString() << "_flag_dr_probe_msa_bin: " << _temp_flag_dr_probe_msa_bin;
    im2model_logger->logEvent(ApplicationLog::notification, message.str());
    message = std::stringstream();
    message << "wavimg path: " <<  _dr_probe_wavimg_bin.toStdString() << "_flag_dr_probe_wavimg_bin: " << _temp_flag_dr_probe_wavimg_bin;
    im2model_logger->logEvent(ApplicationLog::notification, message.str());
  }
  return status;
}

bool MainWindow::readSettings(){
  QSettings settings;

  settings.beginGroup("DrProbe");

  _flag_dr_probe_celslc_bin =  (settings.childKeys().contains("celslc", Qt::CaseInsensitive)) ? true : false;
  _flag_dr_probe_msa_bin =  (settings.childKeys().contains("msa", Qt::CaseInsensitive)) ? true : false;
  _flag_dr_probe_wavimg_bin =  (settings.childKeys().contains("wavimg", Qt::CaseInsensitive)) ? true : false;

  _q_settings_fileName = settings.fileName();
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
  bool result = false;
  if ( ! _was_document_modified() ){
    return  true;
  }
  const QMessageBox::StandardButton ret
    = QMessageBox::warning(this, tr("Application"),
        tr("The document has been modified.\n"
          "Do you want to save your changes?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
  switch (ret) {
    case QMessageBox::Save:
      {
        return save();
      }
    case QMessageBox::Cancel:
      {
        return false;
      }
    default:
      break;
  }
  return true;
}


bool MainWindow::maybeSetProject(){
  bool result = false;
  if ( _flag_project_setted ){
    result =  true;
  }
  else{
    const QMessageBox::StandardButton ret
      = QMessageBox::warning(this, tr("Application"),
          tr("In order to Run Im2Model you need to set the project.\n"
            "Do you want to set the project?"),
          QMessageBox::Yes  | QMessageBox::Cancel);
    switch (ret) {
      case QMessageBox::Yes:
        {
          result = save();
          break;
        }
      case QMessageBox::Cancel:
        {
          result = false;
          break;
        }
      default:
        {
          result = false;
          break;
        }
    }
  }
  return result;
}

void MainWindow::loadFile(const QString &fileName){
#ifndef QT_NO_CURSOR
  QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
  // The first parameter in the constructor is the character used for indentation, whilst the second is the indentation length.
  boost::property_tree::xml_writer_settings<std::string> pt_settings( ' ', 4 );
  boost::property_tree::ptree config;
  bool result = _core_td_map->set_project_filename_with_path( fileName.toStdString() );
  bool missing_version = false;
  bool old_version = false;
  std::string file_version;

  try {
    boost::property_tree::read_xml( fileName.toStdString(), config);
  }
  catch (const boost::property_tree::xml_parser::xml_parser_error& ex) {
    std::cerr << "error in file " << ex.filename() << " line " << ex.line() << std::endl;
    result = false;
  }
  try{
    file_version = config.get<std::string>("version");
    if( application_version > file_version ){
      old_version = true;
      result = false;
    }
  }
  catch(const boost::property_tree::ptree_error &e) {
    missing_version = true;
  }

  if( missing_version ){
    ui->statusBar->showMessage(tr("Version parameter is missing. Will try to load project anyway.") );
  }
  if(result){
    try{
      boost::property_tree::ptree project_setup_image_fields_ptree = config.get_child("project_setup_image_fields_ptree");
      result &= project_setup_image_fields_model->load_data_from_property_tree( project_setup_image_fields_ptree );
      ui->qtree_view_project_setup_image->update();
    }
    catch(const boost::property_tree::ptree_error &e) {
      result = false;
      std::cout << e.what() << std::endl;
    }

    try{
      boost::property_tree::ptree project_setup_crystalographic_fields_ptree = config.get_child("project_setup_crystalographic_fields_ptree");
      result &= project_setup_crystalographic_fields_model->load_data_from_property_tree( project_setup_crystalographic_fields_ptree );
      ui->qtree_view_project_setup_crystallography->update();
    }
    catch(const boost::property_tree::ptree_error &e) {
      result = false;
      std::cout << e.what() << std::endl;
    }

    try{
      boost::property_tree::ptree tdmap_simulation_setup_ptree = config.get_child("tdmap_simulation_setup_ptree");
      result &= tdmap_simulation_setup_model->load_data_from_property_tree( tdmap_simulation_setup_ptree );
      ui->qtree_view_tdmap_simulation_setup->update();
    }
    catch(const boost::property_tree::ptree_error &e) {
      result = false;
      std::cout << e.what() << std::endl;
    }

    try{
      boost::property_tree::ptree tdmap_running_configuration_ptree = config.get_child("tdmap_running_configuration_ptree");
      result &= tdmap_running_configuration_model->load_data_from_property_tree( tdmap_running_configuration_ptree );
      ui->qtree_view_tdmap_running_configuration->update();
    }
    catch(const boost::property_tree::ptree_error &e) {
      result = false;
      std::cout << e.what() << std::endl;
    }

    try{
      boost::property_tree::ptree super_cell_setup_model_ptree = config.get_child("super_cell_setup_model_ptree");
      result &= super_cell_setup_model->load_data_from_property_tree( super_cell_setup_model_ptree );
      ui->qtree_view_supercell_model_edge_detection_setup->update();
    }
    catch(const boost::property_tree::ptree_error &e) {
      result = false;
      std::cout << e.what() << std::endl;
    }
    _flag_project_setted = true;
    this->setCurrentFile(fileName);
  }

#ifndef QT_NO_CURSOR
  QApplication::restoreOverrideCursor();
#endif

  if( result ){
    ui->statusBar->showMessage(tr("Project loaded"), 2000);
  }
  else{
    if( old_version ){
      ui->statusBar->showMessage( QString::fromStdString("Error loading project. File version \"") + QString::fromStdString(file_version) + QString::fromStdString("\" is older than \"") + QString::fromStdString(application_version) + QString::fromStdString("\"" ) );
    }
  }
}

bool MainWindow::saveFile(const QString &fileName ){

#ifndef QT_NO_CURSOR
  QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

  // The first parameter in the constructor is the character used for indentation, whilst the second is the indentation length.
  boost::property_tree::xml_writer_settings<std::string> pt_settings(' ', 4);
  boost::property_tree::ptree *project_setup_image_fields_ptree = project_setup_image_fields_model->save_data_into_property_tree();
  boost::property_tree::ptree *project_setup_crystalographic_fields_ptree = project_setup_crystalographic_fields_model->save_data_into_property_tree();
  boost::property_tree::ptree *tdmap_simulation_setup_ptree = tdmap_simulation_setup_model->save_data_into_property_tree();
  boost::property_tree::ptree *tdmap_running_configuration_ptree = tdmap_running_configuration_model->save_data_into_property_tree();
  boost::property_tree::ptree *super_cell_setup_model_ptree = super_cell_setup_model->save_data_into_property_tree();
  boost::property_tree::ptree *config = new boost::property_tree::ptree();
  config->put( "version", application_version );
  config->add_child("project_setup_image_fields_ptree", *project_setup_image_fields_ptree);
  config->add_child("project_setup_crystalographic_fields_ptree", *project_setup_crystalographic_fields_ptree);
  config->add_child("tdmap_simulation_setup_ptree", *tdmap_simulation_setup_ptree);
  config->add_child("tdmap_running_configuration_ptree", *tdmap_running_configuration_ptree);
  config->add_child("super_cell_setup_model_ptree", *super_cell_setup_model_ptree);

  // Write the property tree to the XML file.
  boost::property_tree::write_xml( fileName.toStdString(), *config, std::locale(), pt_settings );

#ifndef QT_NO_CURSOR
  QApplication::restoreOverrideCursor();
#endif

  setCurrentFile(fileName);
  ui->statusBar->showMessage(tr("Project saved"), 2000);
  _reset_document_modified_flags();
  return true;
}

void MainWindow::setCurrentFile(const QString &fileName){
  curFile = fileName;
  setWindowModified(false);

  QString shownName = curFile;
  if ( !_flag_project_setted ){
    shownName = "*untitled.im2model";
  }

  this->setWindowFilePath(shownName);
  this->setWindowTitle( shownName );
}

QString MainWindow::strippedName(const QString &fullFileName){
  return QFileInfo(fullFileName).fileName();
}

void MainWindow::create_box_options(){

  QVector<QVariant> common_header = {"Field","Value"};

  /*************************
   * EXPERIMENTAL IMAGE
   *************************/
  experimental_image_root = new TreeItem ( common_header );
  experimental_image_root->set_variable_name("experimental_image_root");
  project_setup_image_fields_model = new TreeModel( experimental_image_root );

  ////////////////
  // Image Path
  ////////////////
  QVector<QVariant> box1_option_1 = {"Image path",""};
  QVector<bool> box1_option_1_edit = {false,true};
  boost::function<bool(std::string)> box1_function_1( boost::bind( &MainWindow::update_qline_image_path, this, _1 ) );
  image_path  = new TreeItem (  box1_option_1 , box1_function_1, box1_option_1_edit );
  image_path->set_variable_name( "image_path" );
  image_path->set_item_delegate_type( TreeItem::_delegate_FILE );
  experimental_image_root->insertChildren( image_path );
  /*group options*/
  image_path->set_variable_description( "Experimental image path" );
  simgrid_step_group_options->add_option( project_setup_image_fields_model, image_path , 1, true);

  ////////////////
  // Sampling rate
  ////////////////
  QVector<QVariant> box1_option_2 = {"Pixel size (nm/pixel)",""};
  QVector<QVariant> box1_option_2_1 = {"x",""};
  QVector<bool> box1_option_2_1_edit = {false,true};
  boost::function<bool(std::string)> box1_function_2_1 ( boost::bind( &TDMap::set_exp_image_properties_sampling_rate_x_nm_per_pixel,_core_td_map, _1 ) );
  QVector<QVariant> box1_option_2_2 = {"y",""};
  QVector<bool> box1_option_2_2_edit = {false,true};
  boost::function<bool(std::string)> box1_function_2_2 ( boost::bind( &TDMap::set_exp_image_properties_sampling_rate_y_nm_per_pixel,_core_td_map, _1 ) );

  experimental_sampling_rate = new TreeItem ( box1_option_2  );
  experimental_sampling_rate->set_variable_name( "experimental_sampling_rate" );
  experimental_sampling_rate_x = new TreeItem ( box1_option_2_1 , box1_function_2_1, box1_option_2_1_edit );
  experimental_sampling_rate_x->set_variable_name( "experimental_sampling_rate_x" );
  experimental_sampling_rate_y = new TreeItem ( box1_option_2_2 , box1_function_2_2, box1_option_2_2_edit );
  experimental_sampling_rate_y->set_variable_name( "experimental_sampling_rate_y" );
  experimental_image_root->insertChildren( experimental_sampling_rate );
  experimental_sampling_rate->insertChildren( experimental_sampling_rate_x );
  experimental_sampling_rate->insertChildren( experimental_sampling_rate_y );

  /*group options*/
  celslc_step_group_options->add_option( project_setup_image_fields_model, experimental_sampling_rate_x , 1, true);
  celslc_step_group_options->add_option( project_setup_image_fields_model, experimental_sampling_rate_y , 1, true);
  simgrid_step_group_options->add_option( project_setup_image_fields_model, experimental_sampling_rate_x , 1, true);
  simgrid_step_group_options->add_option( project_setup_image_fields_model, experimental_sampling_rate_y , 1, true);

  /* validators */
  experimental_sampling_rate_x->set_flag_validatable_double(1,true);
  boost::function<double(void)> box1_function_2_1_validator_bot ( boost::bind( &TDMap::get_exp_image_properties_sampling_rate_nm_per_pixel_bottom_limit, _core_td_map ) );
  boost::function<double(void)> box1_function_2_1_validator_top ( boost::bind( &TDMap::get_exp_image_properties_sampling_rate_nm_per_pixel_top_limit, _core_td_map ) );
  experimental_sampling_rate_x->set_validator_double_bottom(1, box1_function_2_1_validator_bot );
  experimental_sampling_rate_x->set_validator_double_top(1, box1_function_2_1_validator_top );

  experimental_sampling_rate_y->set_flag_validatable_double(1,true);
  // same validator as experimental sampling rate x
  experimental_sampling_rate_y->set_validator_double_bottom(1, box1_function_2_1_validator_bot );
  experimental_sampling_rate_y->set_validator_double_top(1, box1_function_2_1_validator_top );

  ////////////////
  // ROI
  ////////////////
  QVector<QVariant> box1_option_3 = {"ROI",""};
  experimental_roi = new TreeItem ( box1_option_3  );
  experimental_roi->set_variable_name( "experimental_roi" );
  experimental_image_root->insertChildren( experimental_roi );

  ////////////////
  // ROI Center
  ////////////////
  QVector<QVariant> box1_option_3_1 = {"Center",""};
  QVector<QVariant> box1_option_3_1_1 = {"x",""};
  QVector<bool> box1_option_3_1_1_edit = {false,true};
  boost::function<bool(std::string)> box1_function_3_1_1 ( boost::bind( &TDMap::set_exp_image_properties_roi_center_x,_core_td_map, _1 ) );
  QVector<QVariant> box1_option_3_1_2 = {"y",""};
  QVector<bool> box1_option_3_1_2_edit = {false,true};
  boost::function<bool(std::string)> box1_function_3_1_2 ( boost::bind( &TDMap::set_exp_image_properties_roi_center_y,_core_td_map, _1 ) );

  experimental_roi_center = new TreeItem ( box1_option_3_1  );
  experimental_roi_center->set_variable_name( "experimental_roi_center" );
  experimental_roi_center_x = new TreeItem ( box1_option_3_1_1 , box1_function_3_1_1, box1_option_3_1_1_edit );
  experimental_roi_center_x->set_variable_name( "experimental_roi_center_x" );
  connect( experimental_roi_center_x, SIGNAL(dataChanged( int )), this, SLOT( update_roi_experimental_image_frame() ) );
  connect( experimental_roi_center_x, SIGNAL(dataChanged( int )), this, SLOT( update_roi_full_experimental_image_frame() ) );

  experimental_roi_center_y = new TreeItem ( box1_option_3_1_2 , box1_function_3_1_2, box1_option_3_1_2_edit );
  experimental_roi_center_y->set_variable_name( "experimental_roi_center_y" );
  connect( experimental_roi_center_y, SIGNAL(dataChanged( int )), this, SLOT( update_roi_experimental_image_frame() ) );
  connect( experimental_roi_center_y, SIGNAL(dataChanged( int )), this, SLOT( update_roi_full_experimental_image_frame() ) );

  experimental_roi->insertChildren( experimental_roi_center );
  experimental_roi_center->insertChildren( experimental_roi_center_x );
  experimental_roi_center->insertChildren( experimental_roi_center_y );

  /*group options*/
  simgrid_step_group_options->add_option( project_setup_image_fields_model, experimental_roi_center_x , 1, true);
  simgrid_step_group_options->add_option( project_setup_image_fields_model, experimental_roi_center_y , 1, true);

  /* validators */
  experimental_roi_center_x->set_flag_validatable_int(1,true);
  boost::function<int(void)> box1_function_3_1_1_validator_bot ( boost::bind( &TDMap::get_experimental_roi_center_x_bottom_limit,_core_td_map ) );
  boost::function<int(void)> box1_function_3_1_1_validator_top ( boost::bind( &TDMap::get_experimental_roi_center_x_top_limit,_core_td_map ) );
  experimental_roi_center_x->set_validator_int_bottom(1, box1_function_3_1_1_validator_bot );
  experimental_roi_center_x->set_validator_int_top(1, box1_function_3_1_1_validator_top );

  experimental_roi_center_y->set_flag_validatable_int(1,true);
  boost::function<int(void)> box1_function_3_1_2_validator_bot ( boost::bind( &TDMap::get_experimental_roi_center_y_bottom_limit,_core_td_map ) );
  boost::function<int(void)> box1_function_3_1_2_validator_top ( boost::bind( &TDMap::get_experimental_roi_center_y_top_limit,_core_td_map ) );
  experimental_roi_center_y->set_validator_int_top(1, box1_function_3_1_2_validator_top );
  experimental_roi_center_y->set_validator_int_bottom(1, box1_function_3_1_2_validator_bot );

  ////////////////
  // ROI Dimensions
  ////////////////

  QVector<QVariant> box1_option_3_2 = {"Dimensions",""};
  experimental_roi_dimensions = new TreeItem ( box1_option_3_2  );
  experimental_roi_dimensions->set_variable_name( "experimental_roi_dimensions" );

  experimental_roi->insertChildren( experimental_roi_dimensions );

  ////////////////
  // Width
  ////////////////
  QVector<QVariant> box1_option_3_2_1 = {"Width",""};
  experimental_roi_dimensions_width  = new TreeItem ( box1_option_3_2_1 );
  experimental_roi_dimensions_width->set_variable_name( "experimental_roi_dimensions_width" );

  experimental_roi_dimensions->insertChildren( experimental_roi_dimensions_width );

  QVector<QVariant> box1_option_3_2_1_1 = {"Pixels",""};
  QVector<bool> box1_option_3_2_1_1_edit = {false,true};
  boost::function<bool(std::string)> box1_function_3_2_1_1 ( boost::bind( &TDMap::set_nx_size_width,_core_td_map, _1 ) );
  experimental_roi_dimensions_width_px = new TreeItem ( box1_option_3_2_1_1 , box1_function_3_2_1_1 , box1_option_3_2_1_1_edit );
  experimental_roi_dimensions_width_px->set_variable_name( "experimental_roi_dimensions_width_px" );
  connect( experimental_roi_dimensions_width_px, SIGNAL(dataChanged( int )), this, SLOT( update_roi_experimental_image_frame() ) );
  connect( experimental_roi_dimensions_width_px, SIGNAL(dataChanged( int )), this, SLOT( update_roi_full_experimental_image_frame() ) );
  experimental_roi_dimensions_width->insertChildren( experimental_roi_dimensions_width_px );

  QVector<QVariant> box1_option_3_2_1_2 = {"nm",""};
  QVector<bool> box1_option_3_2_1_2_edit = {false,false};
  boost::function<double(void)> box1_function_3_2_1_2 ( boost::bind( &TDMap::get_exp_image_properties_roi_nx_size_width_nm,_core_td_map ) );
  experimental_roi_dimensions_width_nm = new TreeItem ( box1_option_3_2_1_2  );
  experimental_roi_dimensions_width_nm->set_variable_name( "experimental_roi_dimensions_width_nm" );
  experimental_roi_dimensions_width_nm->set_fp_data_getter_double_vec( 1, box1_function_3_2_1_2 );
  experimental_roi_dimensions_width->insertChildren( experimental_roi_dimensions_width_nm );
  connect( experimental_roi_dimensions_width_px, SIGNAL(dataChanged( int )), experimental_roi_dimensions_width_nm, SLOT( load_data_from_getter( int ) ) );

  ////////////////
  // Heigth
  ////////////////
  QVector<QVariant> box1_option_3_2_2 = {"Height",""};
  experimental_roi_dimensions_height  = new TreeItem ( box1_option_3_2_2 );
  experimental_roi_dimensions_height->set_variable_name( "experimental_roi_dimensions_height" );
  experimental_roi_dimensions->insertChildren( experimental_roi_dimensions_height );

  QVector<QVariant> box1_option_3_2_2_1 = {"Pixels",""};
  QVector<bool> box1_option_3_2_2_1_edit = {false,true};
  boost::function<bool(std::string)> box1_function_3_2_2_1 ( boost::bind( &TDMap::set_ny_size_height,_core_td_map, _1 ) );
  experimental_roi_dimensions_height_px = new TreeItem ( box1_option_3_2_2_1 , box1_function_3_2_2_1, box1_option_3_2_2_1_edit );
  experimental_roi_dimensions_height_px->set_variable_name( "experimental_roi_dimensions_height_px" );
  connect( experimental_roi_dimensions_height_px, SIGNAL(dataChanged( int )), this, SLOT( update_roi_experimental_image_frame() ) );
  connect( experimental_roi_dimensions_height_px, SIGNAL(dataChanged( int )), this, SLOT( update_roi_full_experimental_image_frame() ) );
  experimental_roi_dimensions_height->insertChildren( experimental_roi_dimensions_height_px );

  QVector<QVariant> box1_option_3_2_2_2 = {"nm",""};
  QVector<bool> box1_option_3_2_2_2_edit = {false,false};
  boost::function<double(void)> box1_function_3_2_2_2 ( boost::bind( &TDMap::get_exp_image_properties_roi_ny_size_height_nm,_core_td_map ) );
  experimental_roi_dimensions_height_nm = new TreeItem ( box1_option_3_2_2_2 );
  experimental_roi_dimensions_height_nm->set_variable_name( "experimental_roi_dimensions_height_nm" );
  experimental_roi_dimensions_height_nm->set_fp_data_getter_double_vec( 1, box1_function_3_2_2_2 );
  experimental_roi_dimensions_height->insertChildren( experimental_roi_dimensions_height_nm );
  connect( experimental_roi_dimensions_height_px, SIGNAL(dataChanged( int )), experimental_roi_dimensions_height_nm, SLOT( load_data_from_getter( int ) ) );

  /*group options*/
  simgrid_step_group_options->add_option( project_setup_image_fields_model, experimental_roi_dimensions_width_px , 1, true);
  simgrid_step_group_options->add_option( project_setup_image_fields_model, experimental_roi_dimensions_height_px , 1, true);

  /* validators */
  experimental_roi_dimensions_width_px->set_flag_validatable_int(1,true);
  experimental_roi_dimensions_height_px->set_flag_validatable_int(1,true);
  boost::function<int(void)> box1_function_3_2_1_validator_bot ( boost::bind( &TDMap::get_experimental_roi_dimensions_width_bottom_limit,_core_td_map ) );
  boost::function<int(void)> box1_function_3_2_1_validator_top ( boost::bind( &TDMap::get_experimental_roi_dimensions_width_top_limit,_core_td_map ) );
  boost::function<int(void)> box1_function_3_2_2_validator_bot ( boost::bind( &TDMap::get_experimental_roi_dimensions_height_bottom_limit,_core_td_map ) );
  boost::function<int(void)> box1_function_3_2_2_validator_top ( boost::bind( &TDMap::get_experimental_roi_dimensions_height_top_limit,_core_td_map ) );

  experimental_roi_dimensions_width_px->set_validator_int_bottom(1, box1_function_3_2_1_validator_bot );
  experimental_roi_dimensions_height_px->set_validator_int_bottom(1, box1_function_3_2_2_validator_bot );

  experimental_roi_dimensions_width_px->set_validator_int_top(1, box1_function_3_2_1_validator_top );
  experimental_roi_dimensions_height_px->set_validator_int_top(1, box1_function_3_2_2_validator_top );

  ui->qtree_view_project_setup_image->setModel(project_setup_image_fields_model);
  ui->qtree_view_project_setup_image->setItemDelegate( _load_file_delegate );
  //start editing after one click
  ui->qtree_view_project_setup_image->setEditTriggers(QAbstractItemView::AllEditTriggers);

  /*************************
   * CRYSTALLOGRAPLY
   *************************/
  crystallography_root = new TreeItem ( common_header );
  crystallography_root->set_variable_name( "crystallography_root" );
  project_setup_crystalographic_fields_model = new TreeModel( crystallography_root );

  ////////////////
  // Unit-cell file
  ////////////////
  QVector<QVariant> box2_option_1 = {"Unit-cell file",""};
  unit_cell_file  = new TreeItem ( box2_option_1 );
  unit_cell_file->set_variable_name( "unit_cell_file" );
  crystallography_root->insertChildren( unit_cell_file );

  ////////////////
  // Unit-cell file CIF
  ////////////////
  QVector<QVariant> box2_option_1_1 = {"CIF",""};
  QVector<bool> box2_option_1_1_edit = {false,true};
  boost::function<bool(std::string)> box2_function_1_1 ( boost::bind( &TDMap::set_unit_cell_cif_path,_core_td_map, _1 ) );
  unit_cell_file_cif = new TreeItem ( box2_option_1_1 , box2_function_1_1, box2_option_1_1_edit );
  unit_cell_file_cif->set_variable_name( "unit_cell_file_cif" );
  unit_cell_file_cif->set_item_delegate_type( TreeItem::_delegate_FILE );
  unit_cell_file->insertChildren( unit_cell_file_cif );

  /*group options*/
  celslc_step_group_options->add_option( project_setup_crystalographic_fields_model, unit_cell_file_cif , 1, true );

  ////////////////
  // Projection direction
  ////////////////
  QVector<QVariant> box2_option_3 = {"Zone axis",""};
  zone_axis  = new TreeItem (  box2_option_3 );
  zone_axis->set_variable_name( "zone_axis" );

  crystallography_root->insertChildren( zone_axis );

  ////////////////
  // Projection direction h
  ////////////////
  QVector<QVariant> box2_option_3_1 = {"u",""};
  QVector<bool> box2_option_3_1_edit = {false,true};
  boost::function<bool(std::string)> box2_function_3_1 ( boost::bind( &TDMap::set_zone_axis_u,_core_td_map, _1 ) );
  zone_axis_u = new TreeItem ( box2_option_3_1 , box2_function_3_1, box2_option_3_1_edit );
  zone_axis_u->set_variable_name( "zone_axis_u" );
  zone_axis->insertChildren( zone_axis_u );

  /*group options*/
  celslc_step_group_options->add_option( project_setup_crystalographic_fields_model, zone_axis_u , 1, true);

  /* validators */
  zone_axis_u->set_flag_validatable_double(1,true);
  ////////////////
  // Projection direction k
  ////////////////
  QVector<QVariant> box2_option_3_2 = {"v",""};
  QVector<bool> box2_option_3_2_edit = {false,true};
  boost::function<bool(std::string)> box2_function_3_2 ( boost::bind( &TDMap::set_zone_axis_v,_core_td_map, _1 ) );
  zone_axis_v = new TreeItem ( box2_option_3_2 , box2_function_3_2, box2_option_3_2_edit );
  zone_axis_v->set_variable_name( "zone_axis_v" );
  zone_axis->insertChildren( zone_axis_v );

  /*group options*/
  celslc_step_group_options->add_option( project_setup_crystalographic_fields_model, zone_axis_v , 1, true);

  /* validators */
  zone_axis_v->set_flag_validatable_double(1,true);

  ////////////////
  // Projection direction l
  ////////////////
  QVector<QVariant> box2_option_3_3 = {"w",""};
  QVector<bool> box2_option_3_3_edit = {false,true};
  boost::function<bool(std::string)> box2_function_3_3 ( boost::bind( &TDMap::set_zone_axis_w,_core_td_map, _1 ) );
  zone_axis_w = new TreeItem ( box2_option_3_3 , box2_function_3_3, box2_option_3_3_edit );
  zone_axis_w->set_variable_name( "zone_axis_w" );
  zone_axis->insertChildren( zone_axis_w );

  /*group options*/
  celslc_step_group_options->add_option( project_setup_crystalographic_fields_model, zone_axis_w , 1, true);

  /* validators */
  zone_axis_w->set_flag_validatable_double(1,true);

  ////////////////
  // Projected y axis
  ////////////////
  QVector<QVariant> box2_option_2 = {"Upward vector",""};
  upward_vector  = new TreeItem (  box2_option_2 );
  upward_vector->set_variable_name( "upward_vector" );
  crystallography_root->insertChildren( upward_vector );

  ////////////////
  //Projected y axis u
  ////////////////
  QVector<QVariant> box2_option_2_1 = {"u",""};
  QVector<bool> box2_option_2_1_edit = {false,true};
  boost::function<bool(std::string)> box2_function_2_1 ( boost::bind( &TDMap::set_upward_vector_u,_core_td_map, _1 ) );
  upward_vector_u = new TreeItem ( box2_option_2_1 , box2_function_2_1, box2_option_2_1_edit );
  upward_vector_u->set_variable_name( "upward_vector_u" );
  upward_vector->insertChildren( upward_vector_u );

  /*group options*/
  celslc_step_group_options->add_option( project_setup_crystalographic_fields_model, upward_vector_u , 1, true);

  /* validators */
  upward_vector_u->set_flag_validatable_double(1,true);

  ////////////////
  //Projected y axis v
  ////////////////
  QVector<QVariant> box2_option_2_2 = {"v",""};
  QVector<bool> box2_option_2_2_edit = {false,true};
  boost::function<bool(std::string)> box2_function_2_2 ( boost::bind( &TDMap::set_upward_vector_v,_core_td_map, _1 ) );
  upward_vector_v = new TreeItem ( box2_option_2_2 , box2_function_2_2, box2_option_2_2_edit );
  upward_vector_v->set_variable_name( "upward_vector_v" );
  upward_vector->insertChildren( upward_vector_v );

  /*group options*/
  celslc_step_group_options->add_option( project_setup_crystalographic_fields_model, upward_vector_v , 1, true);

  /* validators */
  upward_vector_v->set_flag_validatable_double(1,true);

  ////////////////
  //Projected y axis w
  ////////////////
  QVector<QVariant> box2_option_2_3 = {"w",""};
  QVector<bool>  box2_option_2_3_edit = {false,true};
  boost::function<bool(std::string)>  box2_function_2_3 ( boost::bind( &TDMap::set_upward_vector_w,_core_td_map, _1 ) );
  upward_vector_w = new TreeItem (  box2_option_2_3 ,  box2_function_2_3,  box2_option_2_3_edit );
  upward_vector_w->set_variable_name( "upward_vector_w" );
  upward_vector->insertChildren( upward_vector_w );

  /*group options*/
  celslc_step_group_options->add_option( project_setup_crystalographic_fields_model, upward_vector_w , 1, true);

  /* validators */
  upward_vector_w->set_flag_validatable_double(1,true);

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
  tdmap_root = new TreeItem ( common_header );
  tdmap_root->set_variable_name( "tdmap_root" );

  tdmap_simulation_setup_model = new TreeModel( tdmap_root );

  ////////////////
  // 2D variation map
  ////////////////
  QVector<QVariant> box3_option_0 = {"Parameter variation map",""};
  _parameter_variation_map  = new TreeItem ( box3_option_0 );
  _parameter_variation_map->set_variable_name( "_parameter_variation_map" );
  tdmap_root->insertChildren( _parameter_variation_map );

  ////////////////
  // 2D variation map - thickness
  ////////////////
  QVector<QVariant> box3_option_0_1 = {"Thickness",""};
  _parameter_variation_map_thickness  = new TreeItem ( box3_option_0_1 );
  _parameter_variation_map_thickness->set_variable_name( "_parameter_variation_map_thickness" );
  _parameter_variation_map->insertChildren( _parameter_variation_map_thickness );

  /*
  ////////////////
  // 2D variation map - > thickness - > user estimation
  ////////////////
  QVector<QVariant> box3_option_0_1_1 = {"Estimated nm",""};
  QVector<bool> box3_option_0_1_1_edit = {false,true};
  boost::function<bool(std::string)> box3_function_0_1_1 ( boost::bind( &TDMap::set_thickness_user_estimated_nm, _core_td_map, _1 ) );
  _parameter_variation_map_thickness_estimated_nm  = new TreeItem ( box3_option_0_1_1, box3_function_0_1_1, box3_option_0_1_1_edit  );
  _parameter_variation_map_thickness_estimated_nm->set_item_delegate_type( TreeItem::_delegate_TEXT_ACTION );

  QVector<QVariant> box3_function_0_1_1_action_description = {"Auto range","Auto lower/upper"};
  boost::function<bool()> box3_function_0_1_1_auto ( boost::bind( &TDMap::auto_calculate_thickness_range_lower_upper_nm, _core_td_map ) );
  boost::function<bool()> box3_function_0_1_1_auto_lower_upper ( boost::bind( &TDMap::auto_calculate_thickness_lower_upper_nm, _core_td_map ) );

  std::vector<boost::function<bool()>> box3_function_0_1_1_actions;
  box3_function_0_1_1_actions.push_back(box3_function_0_1_1_auto);
  box3_function_0_1_1_actions.push_back(box3_function_0_1_1_auto_lower_upper);

  _parameter_variation_map_thickness_estimated_nm->add_toolbar( box3_function_0_1_1_action_description, box3_function_0_1_1_actions );

  // validators
  _parameter_variation_map_thickness_estimated_nm->set_flag_validatable_double(1,true);

  _parameter_variation_map_thickness->insertChildren( _parameter_variation_map_thickness_estimated_nm );
  */

  ////////////////
  //Thickness range -- Samples
  ////////////////
  QVector<QVariant> box3_option_1_3 = {"Samples",""};
  QVector<bool> box3_option_1_3_edit = {false,true};
  boost::function<bool(std::string)> box3_function_1_3 ( boost::bind( &TDMap::set_slice_samples, _core_td_map, _1 ) );
  thickness_range_number_samples = new TreeItem ( box3_option_1_3 , box3_function_1_3, box3_option_1_3_edit );
  thickness_range_number_samples->set_variable_name( "thickness_range_number_samples" );
  _parameter_variation_map_thickness->insertChildren( thickness_range_number_samples );

  /* validators */
  thickness_range_number_samples->set_flag_validatable_int(1,true);
  boost::function<int(void)> box3_function_1_3_validator_bot ( boost::bind( &TDMap::get_slice_samples_bottom_limit, _core_td_map ) );
  boost::function<int(void)> box3_function_1_3_validator_top ( boost::bind( &TDMap::get_slice_samples_top_limit, _core_td_map ) );
  thickness_range_number_samples->set_validator_int_bottom(1, box3_function_1_3_validator_bot );
  thickness_range_number_samples->set_validator_int_top(1, box3_function_1_3_validator_top );

  ////////////////
  // Thickness range
  ////////////////
  QVector<QVariant> box3_option_1 = {"Thickness range",""};
  thickness_range  = new TreeItem ( box3_option_1 );
  thickness_range->set_variable_name( "thickness_range" );

  _parameter_variation_map_thickness->insertChildren( thickness_range );

  ////////////////
  //Thickness range -- lower bound
  ////////////////
  QVector<QVariant> box3_option_1_1 = {"Lower bound",""};
  QVector<bool> box3_option_1_1_edit = {false,true};
  boost::function<bool(std::string)> box3_function_1_1 ( boost::bind( &TDMap::set_nm_lower_bound, _core_td_map, _1 ) );
  thickness_range_lower_bound = new TreeItem ( box3_option_1_1 , box3_function_1_1, box3_option_1_1_edit );
  thickness_range_lower_bound->set_variable_name( "thickness_range_lower_bound" );
  thickness_range->insertChildren( thickness_range_lower_bound );

  /* validators */
  thickness_range_lower_bound->set_flag_validatable_double(1,true);
  boost::function<double(void)> box3_function_1_1_validator_bot ( boost::bind( &TDMap::get_nm_lower_bound_bottom_limit, _core_td_map ) );
  boost::function<double(void)> box3_function_1_1_validator_top ( boost::bind( &TDMap::get_nm_lower_bound_top_limit, _core_td_map ) );
  thickness_range_lower_bound->set_validator_double_bottom(1, box3_function_1_1_validator_bot );
  thickness_range_lower_bound->set_validator_double_top(1, box3_function_1_1_validator_top );

  ////////////////
  //Thickness range -- upper bound
  ////////////////
  QVector<QVariant> box3_option_1_2 = {"Upper bound",""};
  QVector<bool> box3_option_1_2_edit = {false,true};
  boost::function<bool(std::string)> box3_function_1_2 ( boost::bind( &TDMap::set_nm_upper_bound, _core_td_map, _1 ) );
  thickness_range_upper_bound = new TreeItem ( box3_option_1_2 , box3_function_1_2, box3_option_1_2_edit );
  thickness_range_upper_bound->set_variable_name( "thickness_range_upper_bound" );
  thickness_range->insertChildren( thickness_range_upper_bound );

  /* validators */
  thickness_range_upper_bound->set_flag_validatable_double(1,true);
  boost::function<double(void)> box3_function_1_2_validator_bot ( boost::bind( &TDMap::get_nm_upper_bound_bottom_limit, _core_td_map ) );
  boost::function<double(void)> box3_function_1_2_validator_top ( boost::bind( &TDMap::get_nm_upper_bound_top_limit, _core_td_map ) );
  thickness_range_upper_bound->set_validator_double_bottom(1, box3_function_1_2_validator_bot );
  thickness_range_upper_bound->set_validator_double_top(1, box3_function_1_2_validator_top );

  ////////////////
  // 2D variation map - defocus
  ////////////////
  QVector<QVariant> box3_option_0_2 = {"Defocus",""};
  _parameter_variation_map_defocous  = new TreeItem ( box3_option_0_2 );
  _parameter_variation_map_defocous->set_variable_name( "_parameter_variation_map_defocous" );
  _parameter_variation_map->insertChildren( _parameter_variation_map_defocous );

  ////////////////
  // 2D variation map - > defocus - > user estimation
  ////////////////
  /*
     QVector<QVariant> box3_option_0_2_1 = {"Estimated nm",""};
     QVector<bool> box3_option_0_2_1_edit = {false,true};
     boost::function<bool(std::string)> box3_function_0_2_1 ( boost::bind( &TDMap::set_defocus_user_estimated_nm, _core_td_map, _1 ) );
     _parameter_variation_map_defocus_estimated_nm  = new TreeItem ( box3_option_0_2_1, box3_function_0_2_1, box3_option_0_2_1_edit  );
     _parameter_variation_map_defocous->insertChildren( _parameter_variation_map_defocus_estimated_nm );

  // validators
  _parameter_variation_map_defocus_estimated_nm->set_flag_validatable_double(1,true);
  */

  ////////////////
  //Defocus range -- Samples
  ////////////////
  QVector<QVariant> box3_option_2_3 = {"Samples",""};
  QVector<bool> box3_option_2_3_edit = {false,true};
  boost::function<bool(std::string)> box3_function_2_3 ( boost::bind( &TDMap::set_defocus_samples, _core_td_map, _1 ) );
  defocus_range_number_samples = new TreeItem ( box3_option_2_3 , box3_function_2_3, box3_option_2_3_edit );
  defocus_range_number_samples->set_variable_name( "defocus_range_number_samples" );
  _parameter_variation_map_defocous->insertChildren( defocus_range_number_samples );

  /* validators */
  defocus_range_number_samples->set_flag_validatable_int(1,true);
  boost::function<int(void)> box3_function_2_3_validator_bot ( boost::bind( &TDMap::get_defocus_samples_bottom_limit, _core_td_map ) );
  boost::function<int(void)> box3_function_2_3_validator_top ( boost::bind( &TDMap::get_defocus_samples_top_limit, _core_td_map ) );
  defocus_range_number_samples->set_validator_int_bottom(1, box3_function_2_3_validator_bot );
  defocus_range_number_samples->set_validator_int_top(1, box3_function_2_3_validator_top );

  ////////////////
  // Defocus range
  ////////////////
  QVector<QVariant> box3_option_2 = {"Defocus range",""};
  defocus_range = new TreeItem ( box3_option_2 );
  defocus_range->set_variable_name( "defocus_range" );

  _parameter_variation_map_defocous->insertChildren( defocus_range );

  ////////////////
  //Defocus range -- lower bound
  ////////////////
  QVector<QVariant> box3_option_2_1 = {"Lower bound",""};
  QVector<bool> box3_option_2_1_edit = {false,true};
  boost::function<bool(std::string)> box3_function_2_1 ( boost::bind( &TDMap::set_defocus_lower_bound, _core_td_map, _1 ) );
  defocus_range_lower_bound = new TreeItem ( box3_option_2_1 , box3_function_2_1, box3_option_2_1_edit );
  defocus_range_lower_bound->set_variable_name( "defocus_range_lower_bound" );

  defocus_range->insertChildren( defocus_range_lower_bound );

  /* validators */
  defocus_range_lower_bound->set_flag_validatable_double(1,true);
  boost::function<double(void)> box3_function_2_1_validator_bot ( boost::bind( &TDMap::get_defocus_lower_bound_bottom_limit, _core_td_map ) );
  boost::function<double(void)> box3_function_2_1_validator_top ( boost::bind( &TDMap::get_defocus_lower_bound_top_limit, _core_td_map ) );
  defocus_range_lower_bound->set_validator_double_bottom(1, box3_function_2_1_validator_bot );
  defocus_range_lower_bound->set_validator_double_top(1, box3_function_2_1_validator_top );

  ////////////////
  //Defocus range -- upper bound
  ////////////////
  QVector<QVariant> box3_option_2_2 = {"Upper bound",""};
  QVector<bool> box3_option_2_2_edit = {false,true};
  boost::function<bool(std::string)> box3_function_2_2 ( boost::bind( &TDMap::set_defocus_upper_bound, _core_td_map, _1 ) );
  defocus_range_upper_bound = new TreeItem ( box3_option_2_2 , box3_function_2_2, box3_option_2_2_edit );
  defocus_range_upper_bound->set_variable_name( "defocus_range_upper_bound" );
  defocus_range->insertChildren( defocus_range_upper_bound );

  /* validators */
  defocus_range_upper_bound->set_flag_validatable_double(1,true);
  boost::function<double(void)> box3_function_2_2_validator_bot ( boost::bind( &TDMap::get_defocus_upper_bound_bottom_limit, _core_td_map ) );
  boost::function<double(void)> box3_function_2_2_validator_top ( boost::bind( &TDMap::get_defocus_upper_bound_top_limit, _core_td_map ) );
  defocus_range_upper_bound->set_validator_double_bottom(1, box3_function_2_2_validator_bot );
  defocus_range_upper_bound->set_validator_double_top(1, box3_function_2_2_validator_top );

  ////////////////
  // Incident electron beam
  ////////////////
  QVector<QVariant> box3_option_3 = {"Optics",""};
  incident_electron_beam = new TreeItem ( box3_option_3 );
  incident_electron_beam->set_variable_name( "incident_electron_beam" );
  _parameter_variation_map->insertChildren( incident_electron_beam );

  ////////////////
  // Incident electron beam -- Accelaration voltage (kV)
  ////////////////
  QVector<QVariant> box3_option_3_1 = {"Acceleration voltage (kV)",""};
  QVector<bool> box3_option_3_1_edit = {false,true};
  boost::function<bool(std::string)> box3_function_3_1 ( boost::bind( &TDMap::set_accelaration_voltage_kv, _core_td_map, _1 ) );
  accelaration_voltage_kv = new TreeItem ( box3_option_3_1 , box3_function_3_1, box3_option_3_1_edit );
  accelaration_voltage_kv->set_variable_name( "accelaration_voltage_kv" );
  incident_electron_beam->insertChildren( accelaration_voltage_kv );
  /*group options*/
  celslc_step_group_options->add_option( tdmap_simulation_setup_model, accelaration_voltage_kv , 1, true);
  msa_step_group_options->add_option( tdmap_simulation_setup_model, accelaration_voltage_kv , 1, true);

  /* validators */
  accelaration_voltage_kv->set_flag_validatable_double(1,true);
  boost::function<double(void)> box3_function_3_1_validator_bot ( boost::bind( &TDMap::get_accelaration_voltage_kv_bottom_limit, _core_td_map ) );
  boost::function<double(void)> box3_function_3_1_validator_top ( boost::bind( &TDMap::get_accelaration_voltage_kv_top_limit, _core_td_map ) );
  accelaration_voltage_kv->set_validator_double_bottom(1, box3_function_3_1_validator_bot );
  accelaration_voltage_kv->set_validator_double_top(1, box3_function_3_1_validator_top );

  ////////////////
  // Cell Dimensions
  ////////////////
  QVector<QVariant> box3_option_4 = {"Cell Dimensions",""};
  tdmap_cell_dimensions = new TreeItem ( box3_option_4 );
  tdmap_cell_dimensions->set_variable_name( "tdmap_cell_dimensions" );
  _parameter_variation_map->insertChildren( tdmap_cell_dimensions );

  ////////////////
  // Cell Dimensions -- a
  ////////////////
  QVector<QVariant> box3_option_4_1 = {"a (nm)",""};
  QVector<bool> box3_option_4_1_edit = {false,true};
  boost::function<bool(std::string)> box3_function_4_1 ( boost::bind( &TDMap::set_super_cell_size_a, _core_td_map, _1 ) );
  tdmap_cell_dimensions_a = new TreeItem ( box3_option_4_1 , box3_function_4_1, box3_option_4_1_edit );
  tdmap_cell_dimensions_a->set_variable_name( "tdmap_cell_dimensions_a" );

  tdmap_cell_dimensions->insertChildren( tdmap_cell_dimensions_a );

  /*group options*/
  celslc_step_group_options->add_option( tdmap_simulation_setup_model, tdmap_cell_dimensions_a , 1, true);

  /* validators */
  tdmap_cell_dimensions_a->set_flag_validatable_double(1,true);
  boost::function<double(void)> box3_function_4_1_validator_bot ( boost::bind( &TDMap::get_tdmap_cell_dimensions_a_bottom_limit, _core_td_map ) );
  boost::function<double(void)> box3_function_4_1_validator_top ( boost::bind( &TDMap::get_tdmap_cell_dimensions_a_top_limit, _core_td_map ) );
  tdmap_cell_dimensions_a->set_validator_double_bottom(1, box3_function_4_1_validator_bot );
  tdmap_cell_dimensions_a->set_validator_double_top(1, box3_function_4_1_validator_top );

  ////////////////
  // Cell Dimensions -- b
  ////////////////
  QVector<QVariant> box3_option_4_2 = {"b (nm)",""};
  QVector<bool> box3_option_4_2_edit = {false,true};
  boost::function<bool(std::string)> box3_function_4_2 ( boost::bind( &TDMap::set_super_cell_size_b, _core_td_map, _1 ) );
  tdmap_cell_dimensions_b = new TreeItem ( box3_option_4_2 , box3_function_4_2, box3_option_4_2_edit );
  tdmap_cell_dimensions_b->set_variable_name( "tdmap_cell_dimensions_b" );
  tdmap_cell_dimensions->insertChildren( tdmap_cell_dimensions_b );

  /*group options*/
  celslc_step_group_options->add_option( tdmap_simulation_setup_model, tdmap_cell_dimensions_b , 1, true);

  /* validators */
  tdmap_cell_dimensions_b->set_flag_validatable_double(1,true);
  boost::function<double(void)> box3_function_4_2_validator_bot ( boost::bind( &TDMap::get_tdmap_cell_dimensions_b_bottom_limit, _core_td_map ) );
  boost::function<double(void)> box3_function_4_2_validator_top ( boost::bind( &TDMap::get_tdmap_cell_dimensions_b_top_limit, _core_td_map ) );
  tdmap_cell_dimensions_b->set_validator_double_bottom(1, box3_function_4_2_validator_bot );
  tdmap_cell_dimensions_b->set_validator_double_top(1, box3_function_4_2_validator_top );

  ////////////////
  // Simulation Refinement
  ////////////////
  QVector<QVariant> box3_option_5 = {"Refinement",""};
  QVector<bool> box3_option_5_edit = {false,true};

  boost::function<int(void)> box3_function_5_getter ( boost::bind( &TDMap::get_refinement_definition_method, _core_td_map ) );
  boost::function<bool(int)> box3_function_5_setter ( boost::bind( &TDMap::set_refinement_definition_method, _core_td_map, _1 ) );

  _simulation_refinement = new TreeItem ( box3_option_5, box3_function_5_setter, box3_function_5_getter, box3_option_5_edit );
  _simulation_refinement->set_variable_name( "_simulation_refinement" );

  // load the preset data from core constuctor
  _simulation_refinement->load_data_from_getter();
  QVector<QVariant> box3_option_5_drop = {"No refinement","Corrected","Non-Corrected", "User defined"};

  QVector<QVariant> box3_option_5_drop_enum( { TDMap::RefinementPreset::NO_REFINEMENT, TDMap::RefinementPreset::MICROSCOPE_CORRECTED, TDMap::RefinementPreset::MICROSCOPE_NON_CORRECTED, TDMap::RefinementPreset::USER_DEFINED_PRESET } );

  _simulation_refinement->set_item_delegate_type( TreeItem::_delegate_DROP );
  _simulation_refinement->set_dropdown_options( 1, box3_option_5_drop, box3_option_5_drop_enum  );

  tdmap_root->insertChildren( _simulation_refinement );

  ////////////////
  //Simulation Refinement -- Aberration parameters
  ////////////////
  QVector<QVariant> box3_option_5_1 = {"Aberration parameters",""};
  _aberration_parameters = new TreeItem ( box3_option_5_1 );
  _aberration_parameters->set_variable_name( "_aberration_parameters" );

  _simulation_refinement->insertChildren( _aberration_parameters );

  ////////////////
  // Aberration parameters -- Spherical aberration (a40, C3,0, C3)
  ////////////////
  QVector<QVariant> box3_option_5_1_1 = {"Spherical aberration (nm)",""};
  QVector<bool> box3_option_5_1_1_edit = {false,true};
  boost::function<bool(std::string)> box3_function_5_1_1_setter ( boost::bind( &TDMap::set_spherical_aberration, _core_td_map, _1 ) );
  boost::function<double(void)> box3_function_5_1_1_getter ( boost::bind( &TDMap::get_spherical_aberration, _core_td_map ) );
  boost::function<bool(void)> box3_option_5_1_1_check_getter ( boost::bind( &TDMap::get_spherical_aberration_switch, _core_td_map  ) );
  boost::function<bool(bool)> box3_option_5_1_1_check_setter ( boost::bind( &TDMap::set_spherical_aberration_switch, _core_td_map, _1 ) );
  spherical_aberration_nm = new TreeItem ( box3_option_5_1_1 , box3_function_5_1_1_setter, box3_function_5_1_1_getter, box3_option_5_1_1_edit );
  spherical_aberration_nm->set_variable_name( "spherical_aberration_nm" );

  _aberration_parameters->insertChildren( spherical_aberration_nm );

  /*group options*/
  spherical_aberration_nm->set_fp_check_setter( 0, box3_option_5_1_1_check_setter );
  spherical_aberration_nm->set_fp_check_getter( 0, box3_option_5_1_1_check_getter );
  spherical_aberration_nm->load_check_status_from_getter( 0 );
  /* validators */
  spherical_aberration_nm->set_flag_validatable_double(1,true);

  ////////////////
  //Simulation Refinement -- envelope parameters
  ////////////////
  QVector<QVariant> box3_option_5_2 = {"Envelope parameters",""};
  _envelope_parameters = new TreeItem ( box3_option_5_2 );
  _envelope_parameters->set_variable_name( "_envelope_parameters" );

  _simulation_refinement->insertChildren( _envelope_parameters );

  QVector<QVariant> box3_option_5_2_1 = {"Image spread",""};
  QVector<bool> box3_option_5_2_1_edit = {false,true};

  boost::function<int(void)> box3_function_5_2_1_getter ( boost::bind( &TDMap::get_envelop_parameters_vibrational_damping_method, _core_td_map ) );
  boost::function<bool(int)> box3_function_5_2_1_setter ( boost::bind( &TDMap::set_envelop_parameters_vibrational_damping_method, _core_td_map, _1 ) );

  _envelope_parameters_vibrational_damping = new TreeItem ( box3_option_5_2_1, box3_function_5_2_1_setter, box3_function_5_2_1_getter, box3_option_5_edit );
  _envelope_parameters_vibrational_damping->set_variable_name( "_envelope_parameters_vibrational_damping" );

  // load the preset data from core constuctor
  _envelope_parameters_vibrational_damping->load_data_from_getter();
  QVector<QVariant> box3_option_5_2_1_drop = {"Deactivated","Isotropic","Anisotropic"};
  QVector<QVariant> box3_option_5_2_1_drop_enum( { WAVIMG_prm::EnvelopeVibrationalDamping::Deactivated, WAVIMG_prm::EnvelopeVibrationalDamping::Isotropic, WAVIMG_prm::EnvelopeVibrationalDamping::Anisotropic } );
  _envelope_parameters_vibrational_damping->set_item_delegate_type( TreeItem::_delegate_DROP );
  _envelope_parameters_vibrational_damping->set_dropdown_options( 1, box3_option_5_2_1_drop, box3_option_5_2_1_drop_enum  );

  _envelope_parameters->insertChildren( _envelope_parameters_vibrational_damping );

  //  bool set_envelop_parameters_vibrational_damping_anisotropic_second_rms_amplitude( double amplitude );
  //  bool set_envelop_parameters_vibrational_damping_azimuth_orientation_angle( double angle );

  ////////////////
  // Image spread -- first rms (nm)
  ////////////////
  QVector<QVariant> box3_option_5_2_1_1 = {"First rms (nm)",""};
  QVector<bool> box3_option_5_2_1_1_edit = {false,true};
  boost::function<bool(double)> box3_function_5_2_1_1 ( boost::bind( &TDMap::set_envelop_parameters_vibrational_damping_isotropic_one_rms_amplitude, _core_td_map, _1 ) );
  envelop_parameters_vibrational_damping_isotropic_first_rms_amplitude = new TreeItem ( box3_option_5_2_1_1 , box3_function_5_2_1_1, box3_option_5_2_1_1_edit );
  envelop_parameters_vibrational_damping_isotropic_first_rms_amplitude->set_variable_name( "envelop_parameters_vibrational_damping_isotropic_first_rms_amplitude" );

  _envelope_parameters_vibrational_damping->insertChildren( envelop_parameters_vibrational_damping_isotropic_first_rms_amplitude );
  /* validators */
  envelop_parameters_vibrational_damping_isotropic_first_rms_amplitude->set_flag_validatable_double(1,true);

  ////////////////
  // Image spread -- second rms (nm)
  ////////////////
  QVector<QVariant> box3_option_5_2_1_2 = {"Second rms (nm)",""};
  QVector<bool> box3_option_5_2_1_2_edit = {false,true};
  boost::function<bool(double)> box3_function_5_2_1_2 ( boost::bind( &TDMap::set_envelop_parameters_vibrational_damping_anisotropic_second_rms_amplitude, _core_td_map, _1 ) );
  envelop_parameters_vibrational_damping_isotropic_second_rms_amplitude = new TreeItem ( box3_option_5_2_1_2 , box3_function_5_2_1_2, box3_option_5_2_1_2_edit );
  envelop_parameters_vibrational_damping_isotropic_second_rms_amplitude->set_variable_name( "envelop_parameters_vibrational_damping_isotropic_second_rms_amplitude" );
  _envelope_parameters_vibrational_damping->insertChildren( envelop_parameters_vibrational_damping_isotropic_second_rms_amplitude );
  /* validators */
  envelop_parameters_vibrational_damping_isotropic_second_rms_amplitude->set_flag_validatable_double(1,true);

  ////////////////
  // Image spread -- orientation angle
  ////////////////
  QVector<QVariant> box3_option_5_2_1_3 = {"Orientation angle",""};
  QVector<bool> box3_option_5_2_1_3_edit = {false,true};
  boost::function<bool(double)> box3_function_5_2_1_3 ( boost::bind( &TDMap::set_envelop_parameters_vibrational_damping_azimuth_orientation_angle, _core_td_map, _1 ) );
  envelop_parameters_vibrational_damping_isotropic_orientation_angle = new TreeItem ( box3_option_5_2_1_3 , box3_function_5_2_1_3, box3_option_5_2_1_3_edit );
  envelop_parameters_vibrational_damping_isotropic_orientation_angle->set_variable_name( "envelop_parameters_vibrational_damping_isotropic_orientation_angle" );
  _envelope_parameters_vibrational_damping->insertChildren( envelop_parameters_vibrational_damping_isotropic_orientation_angle );
  /* validators */
  envelop_parameters_vibrational_damping_isotropic_orientation_angle->set_flag_validatable_double(1,true);


  ////////////////
  // Envelop parameters - temporal coherence
  ////////////////
  QVector<QVariant> box3_option_5_2_2 = {"Temporal coherence - focus-spread",""};
  QVector<bool> box3_option_5_2_2_edit = {false,true};
  boost::function<bool(std::string)> box3_function_5_2_2_setter ( boost::bind( &TDMap::set_partial_temporal_coherence_focus_spread, _core_td_map, _1 ) );
  boost::function<bool(void)> box3_option_5_2_2_check_getter ( boost::bind( &TDMap::get_partial_temporal_coherence_switch, _core_td_map  ) );
  boost::function<bool(bool)> box3_option_5_2_2_check_setter ( boost::bind( &TDMap::set_partial_temporal_coherence_switch, _core_td_map, _1 ) );
  partial_temporal_coherence_focus_spread = new TreeItem ( box3_option_5_2_2 , box3_function_5_2_2_setter, box3_option_5_2_2_edit );
  partial_temporal_coherence_focus_spread->set_variable_name( "partial_temporal_coherence_focus_spread" );
  _envelope_parameters->insertChildren( partial_temporal_coherence_focus_spread );
  /* validators */
  partial_temporal_coherence_focus_spread->set_flag_validatable_double(1,true);

  /*group options*/
  partial_temporal_coherence_focus_spread->set_variable_name( "partial_temporal_coherence_focus_spread" );
  partial_temporal_coherence_focus_spread->set_fp_check_setter( 0, box3_option_5_2_2_check_setter );
  partial_temporal_coherence_focus_spread->set_fp_check_getter( 0, box3_option_5_2_2_check_getter );
  partial_temporal_coherence_focus_spread->load_check_status_from_getter( 0 );

  ////////////////
  // Envelop parameters - spatial coherence
  ////////////////
  QVector<QVariant> box3_option_5_2_3 = {"Spatial coherence - semi-convergence angle",""};
  QVector<bool> box3_option_5_2_3_edit = {false,true};
  boost::function<bool(std::string)> box3_function_5_2_3_setter ( boost::bind( &TDMap::set_partial_spatial_coherence_semi_convergence_angle, _core_td_map, _1 ) );
  boost::function<bool(void)> box3_option_5_2_3_check_getter ( boost::bind( &TDMap::get_partial_spatial_coherence_switch, _core_td_map  ) );
  boost::function<bool(bool)> box3_option_5_2_3_check_setter ( boost::bind( &TDMap::set_partial_spatial_coherence_switch, _core_td_map, _1 ) );
  partial_spatial_coherence_semi_convergence_angle = new TreeItem ( box3_option_5_2_3 , box3_function_5_2_3_setter, box3_option_5_2_3_edit );
  partial_spatial_coherence_semi_convergence_angle->set_variable_name( "partial_spatial_coherence_semi_convergence_angle" );
  _envelope_parameters->insertChildren( partial_spatial_coherence_semi_convergence_angle );

  /* validators */
  partial_spatial_coherence_semi_convergence_angle->set_flag_validatable_double(1,true);
  /*group options*/
  partial_spatial_coherence_semi_convergence_angle->set_fp_check_setter( 0, box3_option_5_2_3_check_setter );
  partial_spatial_coherence_semi_convergence_angle->set_fp_check_getter( 0, box3_option_5_2_3_check_getter );
  partial_spatial_coherence_semi_convergence_angle->load_check_status_from_getter( 0 );

  ////////////////
  //Simulation Refinement -- MTF
  ////////////////
  QVector<QVariant> box3_option_5_3 = {"MTF",""};
  QVector<bool> box3_option_5_3_edit = {false,true};
  boost::function<bool(std::string)> box3_function_5_3 ( boost::bind( &TDMap::set_mtf_filename,_core_td_map, _1 ) );
  _mtf_parameters = new TreeItem ( box3_option_5_3 , box3_function_5_3, box3_option_5_3_edit );
  _mtf_parameters->set_variable_name( "_mtf_parameters" );
  _mtf_parameters->set_item_delegate_type( TreeItem::_delegate_FILE );
  _simulation_refinement->insertChildren( _mtf_parameters );

  boost::function<bool(void)> box3_option_5_3_1_check_getter ( boost::bind( &TDMap::get_mtf_switch, _core_td_map  ) );
  boost::function<bool(bool)> box3_option_5_3_1_check_setter ( boost::bind( &TDMap::set_mtf_switch, _core_td_map, _1 ) );
  _mtf_parameters->set_fp_check_setter( 0, box3_option_5_3_1_check_setter );
  _mtf_parameters->set_fp_check_getter( 0, box3_option_5_3_1_check_getter );
  _mtf_parameters->load_check_status_from_getter( 0 );

  ////////////////
  // Image Correlation
  ////////////////
  QVector<QVariant> box3_option_6 = {"Image correlation",""};
  image_correlation  = new TreeItem ( box3_option_6 );
  image_correlation->set_variable_name( "image_correlation" );
  tdmap_root->insertChildren( image_correlation );

  ////////////////
  // Image Correlation -- Hysteresis Thresholding
  ////////////////
  //: , non-normalized correlation and sum-absolute-difference

  QVector<QVariant> box3_option_6_1 = {"Match method",""};
  QVector<bool> box3_option_6_1_edit = {false,true};

  boost::function<int(void)> box3_function_6_1_getter ( boost::bind( &TDMap::get_image_correlation_matching_method, _core_td_map ) );
  boost::function<bool(int)> box3_function_6_1_setter ( boost::bind( &TDMap::set_image_correlation_matching_method, _core_td_map, _1 ) );
  image_correlation_matching_method = new TreeItem ( box3_option_6_1 , box3_function_6_1_setter, box3_function_6_1_getter,  box3_option_6_1_edit );
  image_correlation_matching_method->set_variable_name( "image_correlation_matching_method" );

  // load the preset data from core constuctor
  image_correlation_matching_method->load_data_from_getter();

  QVector<QVariant> box3_option_6_1_drop = {"Normalized squared difference","Normalized cross correlation","Normalized correlation coefficient"};
  QVector<QVariant> box3_option_6_1_drop_enum( { CV_TM_SQDIFF_NORMED, CV_TM_CCORR_NORMED, CV_TM_CCOEFF_NORMED} );

  image_correlation_matching_method->set_item_delegate_type( TreeItem::_delegate_DROP );
  image_correlation_matching_method->set_dropdown_options( 1, box3_option_6_1_drop, box3_option_6_1_drop_enum  );

  image_correlation->insertChildren( image_correlation_matching_method );


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
  running_configuration_root->set_variable_name( "running_configuration_root" );

  tdmap_running_configuration_model = new TreeModel( running_configuration_root );

  ////////////////
  // Log level
  ////////////////

  QVector<QVariant> box4_option_0 = {"", "Log level"};
  TreeItem* _log_level  = new TreeItem ( box4_option_0 );
  _log_level->set_variable_name( "_log_level" );

  running_configuration_root->insertChildren( _log_level );

  QVector<QVariant> box4_option_1 = {"", ""};
  QVector<bool> box4_option_1_edit = {false,true};
  boost::function<int()> box4_function_1_getter ( boost::bind( &TDMap::get_exec_log_level, _core_td_map ) );
  boost::function<bool(int)> box4_function_1_setter ( boost::bind( &TDMap::set_exec_log_level, _core_td_map, _1 ) );

  TreeItem* _log_level_setter  = new TreeItem ( box4_option_1, box4_function_1_setter, box4_function_1_getter, box4_option_1_edit );
  _log_level_setter->set_variable_name( "_log_level_setter" );

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
  QVector<QVariant> box4_data_2 = {"","Multislice phase granting"};
  boost::function<bool(void)> box4_option_2_check_getter ( boost::bind( &TDMap::get_run_celslc_switch, _core_td_map  ) );
  boost::function<bool(bool)> box4_option_2_check_setter ( boost::bind( &TDMap::set_run_celslc_switch, _core_td_map, _1 ) );
  QVector<bool> box4_option_2_edit = {false,false};
  TreeItem* _multislice_phase_granting  = new TreeItem ( box4_data_2 , box4_option_2_edit );
  _multislice_phase_granting->set_variable_name( "_multislice_phase_granting" );

  //_multislice_phase_granting->setStatusOption( 0, TreeItem::ActionStatusType::_status_NOT_READY );
  // load the preset data from core constuctor
  _multislice_phase_granting->set_fp_check_setter( 1, box4_option_2_check_setter );
  _multislice_phase_granting->set_fp_check_getter( 1, box4_option_2_check_getter );
  _multislice_phase_granting->load_check_status_from_getter( 1 );

  running_configuration_root->insertChildren( _multislice_phase_granting );


  QVector<QVariant> box4_option_2_0 = {"", "Output"};
  TreeItem* _multislice_phase_granting_output_legend   = new TreeItem ( box4_option_2_0 );
  _multislice_phase_granting_output_legend->set_variable_name( "_multislice_phase_granting_output_legend" );
  _multislice_phase_granting->insertChildren( _multislice_phase_granting_output_legend );

  QVector<QVariant> box4_option_2_1 = {"", ""};
  QVector<bool> box4_option_2_1_edit = {false,true};
  _multislice_phase_granting_output  = new TreeItem ( box4_option_2_1, box4_option_2_1_edit );
  _multislice_phase_granting_output->set_variable_name( "_multislice_phase_granting_output" );

  _multislice_phase_granting_output->set_fp_data_data_appender_col_pos( 1 );
  _multislice_phase_granting_output->set_flag_fp_data_appender_string( true );
  _multislice_phase_granting_output->set_item_delegate_type( TreeItem::_delegate_TEXT_BROWSER );
  _multislice_phase_granting_output_legend->insertChildren( _multislice_phase_granting_output );

  QVector<QVariant> box4_option_2_2 = {"","Temporary files"};
  TreeItem* _multislice_phase_granting_temporary_files  = new TreeItem ( box4_option_2_2 );
  _multislice_phase_granting_temporary_files->set_variable_name( "_multislice_phase_granting_temporary_files" );
  _multislice_phase_granting->insertChildren( _multislice_phase_granting_temporary_files );

  /*
   * MSA
   * */

  QVector<QVariant> box4_data_3 = {"","Electron diffraction patterns"};
  boost::function<bool(void)> box4_option_3_check_getter ( boost::bind( &TDMap::get_run_msa_switch, _core_td_map  ) );
  boost::function<bool(bool)> box4_option_3_check_setter ( boost::bind( &TDMap::set_run_msa_switch, _core_td_map, _1 ) );
  QVector<bool> box4_option_3_edit = {false,false};
  TreeItem* _electron_diffraction_patterns  = new TreeItem ( box4_data_3 , box4_option_3_edit );
  _electron_diffraction_patterns->set_variable_name( "_electron_diffraction_patterns" );

  _multislice_phase_granting->setStatusOption( 0, TreeItem::ActionStatusType::_status_NOT_READY );
  // load the preset data from core constuctor
  _electron_diffraction_patterns->set_fp_check_setter( 1, box4_option_3_check_setter );
  _electron_diffraction_patterns->set_fp_check_getter( 1, box4_option_3_check_getter );
  _electron_diffraction_patterns->load_check_status_from_getter( 1 );

  running_configuration_root->insertChildren( _electron_diffraction_patterns );

  QVector<QVariant> box4_option_3_0 = {"", "Output"};
  TreeItem* _electron_diffraction_patterns_output_legend   = new TreeItem ( box4_option_3_0 );
  _electron_diffraction_patterns_output_legend->set_variable_name( "_electron_diffraction_patterns_output_legend" );

  _electron_diffraction_patterns->insertChildren( _electron_diffraction_patterns_output_legend );

  QVector<QVariant> box4_option_3_1 = {"", ""};
  QVector<bool> box4_option_3_1_edit = {false,true};
  _electron_diffraction_patterns_output  = new TreeItem ( box4_option_3_1, box4_option_3_1_edit );
  _electron_diffraction_patterns_output->set_variable_name( "_electron_diffraction_patterns_output" );

  _electron_diffraction_patterns_output->set_fp_data_data_appender_col_pos( 1 );
  _electron_diffraction_patterns_output->set_flag_fp_data_appender_string( true );
  _electron_diffraction_patterns_output->set_item_delegate_type( TreeItem::_delegate_TEXT_BROWSER );
  _electron_diffraction_patterns_output_legend->insertChildren( _electron_diffraction_patterns_output );

  QVector<QVariant> box4_option_3_2 = {"","Temporary files"};
  TreeItem* _electron_diffraction_patterns_temporary_files  = new TreeItem ( box4_option_3_2 );
  _electron_diffraction_patterns_temporary_files->set_variable_name( "_electron_diffraction_patterns_temporary_files" );

  _electron_diffraction_patterns->insertChildren( _electron_diffraction_patterns_temporary_files );

  /*
   * WAVIMG
   * */
  QVector<QVariant> box4_data_4 = {"","Image intensity distribuitions"};
  boost::function<bool(void)> box4_option_4_check_getter ( boost::bind( &TDMap::get_run_wavimg_switch, _core_td_map  ) );
  boost::function<bool(bool)> box4_option_4_check_setter ( boost::bind( &TDMap::set_run_wavimg_switch, _core_td_map, _1 ) );
  QVector<bool> box4_option_4_edit = {false,false};
  TreeItem* _image_intensity_distribuitions  = new TreeItem ( box4_data_4 , box4_option_4_edit );
  _image_intensity_distribuitions->set_variable_name( "_image_intensity_distribuitions" );

  _image_intensity_distribuitions->setStatusOption( 0, TreeItem::ActionStatusType::_status_NOT_READY );
  // load the preset data from core constuctor
  _image_intensity_distribuitions->set_fp_check_setter( 1, box4_option_4_check_setter );
  _image_intensity_distribuitions->set_fp_check_getter( 1, box4_option_4_check_getter );
  _image_intensity_distribuitions->load_check_status_from_getter( 1 );

  running_configuration_root->insertChildren( _image_intensity_distribuitions );

  QVector<QVariant> box4_option_4_0 = {"","Output"};
  TreeItem* _image_intensity_distribuitions_output_legend   = new TreeItem ( box4_option_4_0 );
  _image_intensity_distribuitions_output_legend->set_variable_name( "_image_intensity_distribuitions_output_legend" );

  _image_intensity_distribuitions->insertChildren( _image_intensity_distribuitions_output_legend );

  QVector<QVariant> box4_option_4_1 = {"", ""};
  QVector<bool> box4_option_4_1_edit = {false,true};
  _image_intensity_distribuitions_output  = new TreeItem ( box4_option_4_1, box4_option_4_1_edit );
  _image_intensity_distribuitions_output->set_variable_name( "_image_intensity_distribuitions_output" );

  _image_intensity_distribuitions_output->set_fp_data_data_appender_col_pos( 1 );
  _image_intensity_distribuitions_output->set_flag_fp_data_appender_string( true );
  _image_intensity_distribuitions_output->set_item_delegate_type( TreeItem::_delegate_TEXT_BROWSER );
  _image_intensity_distribuitions_output_legend->insertChildren( _image_intensity_distribuitions_output );

  QVector<QVariant> box4_option_4_2 = {"","Temporary files"};
  TreeItem* _image_intensity_distribuitions_temporary_files  = new TreeItem ( box4_option_4_2 );
  _image_intensity_distribuitions_temporary_files->set_variable_name( "_image_intensity_distribuitions_temporary_files" );

  _image_intensity_distribuitions->insertChildren( _image_intensity_distribuitions_temporary_files );

  /*
   * SIMGRID
   * */
  QVector<QVariant> box4_data_5 = {"","Image correlation"};
  boost::function<bool(void)> box4_option_5_check_getter ( boost::bind( &TDMap::get_run_simgrid_switch, _core_td_map  ) );
  boost::function<bool(bool)> box4_option_5_check_setter ( boost::bind( &TDMap::set_run_simgrid_switch, _core_td_map, _1 ) );
  QVector<bool> box4_option_5_edit = {false,false};
  TreeItem* _image_correlation  = new TreeItem ( box4_data_5 , box4_option_5_edit );
  _image_correlation->set_variable_name( "_image_correlation" );

  _image_correlation->setStatusOption( 0, TreeItem::ActionStatusType::_status_NOT_READY );
  // load the preset data from core constuctor
  _image_correlation->set_fp_check_setter( 1, box4_option_5_check_setter );
  _image_correlation->set_fp_check_getter( 1, box4_option_5_check_getter );
  _image_correlation->load_check_status_from_getter( 1 );
  running_configuration_root->insertChildren( _image_correlation );

  QVector<QVariant> box4_option_5_0 = {"","Output"};
  TreeItem* _image_correlation_output_legend  = new TreeItem ( box4_option_5_0 );
  _image_correlation_output_legend->set_variable_name( "_image_correlation_output_legend" );

  _image_correlation->insertChildren( _image_correlation_output_legend );

  QVector<QVariant> box4_option_5_1 = {"", ""};
  QVector<bool> box4_option_5_1_edit = {false,true};
  _image_correlation_output  = new TreeItem ( box4_option_5_1, box4_option_5_1_edit );
  _image_correlation_output->set_variable_name( "_image_correlation_output" );

  _image_correlation_output->set_fp_data_data_appender_col_pos( 1 );
  _image_correlation_output->set_flag_fp_data_appender_string( true );
  _image_correlation_output->set_item_delegate_type( TreeItem::_delegate_TEXT_BROWSER );
  _image_correlation_output_legend->insertChildren( _image_correlation_output );

  ui->qtree_view_tdmap_running_configuration->setModel( tdmap_running_configuration_model );
  ui->qtree_view_tdmap_running_configuration->setItemDelegate( _load_file_delegate );
  //start editing after one click
  ui->qtree_view_tdmap_running_configuration->setEditTriggers(QAbstractItemView::AllEditTriggers);
  for (int column = 0; column < tdmap_simulation_setup_model->columnCount(); ++column){
    ui->qtree_view_tdmap_running_configuration->resizeColumnToContents(column);
  }

  /*************************
   * CRYSTALLOGRAPLY
   *************************/
  TreeItem* super_cell_setup_root = new TreeItem ( common_header );
  super_cell_setup_root->set_variable_name( "super_cell_setup_root" );
  super_cell_setup_model = new TreeModel( super_cell_setup_root );

  ////////////////
  // Edge detection
  ////////////////
  QVector<QVariant> box5_option_1 = {"Edge detection",""};
  TreeItem* edge_detection  = new TreeItem ( box5_option_1 );
  edge_detection->set_variable_name( "edge_detection" );

  super_cell_setup_root->insertChildren( edge_detection );

  QVector<QVariant> box5_option_1_data_1 = {"Hysteresis thresholding",""};
  boost::function<int(void)> box5_option_1_check_getter ( boost::bind( &TDMap::get_exp_image_bounds_hysteresis_threshold, _core_td_map ) );
  boost::function<bool(int)> box5_option_1_check_setter ( boost::bind( &TDMap::set_exp_image_bounds_hysteresis_threshold, _core_td_map, _1 ) );
  QVector<bool> box5_option_1_edit = {false,true};
  TreeItem* _hysteris_thresholding  = new TreeItem ( box5_option_1_data_1 ,box5_option_1_check_setter, box5_option_1_check_getter, box5_option_1_edit );
  _hysteris_thresholding->set_variable_name( "_hysteris_thresholding" );

  _hysteris_thresholding->set_item_delegate_type( TreeItem::_delegate_SLIDER_INT );
  // load the preset data from core constuctor
  _hysteris_thresholding->load_data_from_getter();
  // set the bottom and top limits of the interval
  int hysteresis_threshold_bottom_limit = _core_td_map->get_exp_image_bounds_hysteresis_threshold_range_bottom_limit( );
  int hysteresis_threshold_top_limit = _core_td_map->get_exp_image_bounds_hysteresis_threshold_range_top_limit( );
  _hysteris_thresholding->set_slider_int_range_min( hysteresis_threshold_bottom_limit );
  _hysteris_thresholding->set_slider_int_range_max( hysteresis_threshold_top_limit );

  edge_detection->insertChildren( _hysteris_thresholding );

  QVector<QVariant> box5_option_1_data_2 = {"Max. contour distance",""};
  boost::function<int(void)> box5_option_1_2_check_getter ( boost::bind( &TDMap::get_exp_image_bounds_max_contour_distance_px, _core_td_map ) );
  boost::function<bool(int)> box5_option_1_2_check_setter ( boost::bind( &TDMap::set_exp_image_bounds_max_contour_distance_px, _core_td_map, _1 ) );
  QVector<bool> box5_option_1_2_edit = {false,true};
  TreeItem* _max_contour_distance  = new TreeItem ( box5_option_1_data_2 ,box5_option_1_2_check_setter, box5_option_1_2_check_getter, box5_option_1_2_edit );
  _max_contour_distance->set_variable_name( "_max_contour_distance" );

  _max_contour_distance->set_item_delegate_type( TreeItem::_delegate_SLIDER_INT );
  // load the preset data from core constuctor
  _max_contour_distance->load_data_from_getter();
  // set the bottom and top limits of the interval
  int max_contour_distance_bottom_limit =  _core_td_map->get_exp_image_bounds_max_contour_distance_px_range_bottom_limit( );
  int max_contour_distance_top_limit =  _core_td_map->get_exp_image_bounds_max_contour_distance_px_range_top_limit( );
  _max_contour_distance->set_slider_int_range_min( max_contour_distance_bottom_limit );
  _max_contour_distance->set_slider_int_range_max( max_contour_distance_top_limit );

  edge_detection->insertChildren( _max_contour_distance );
  //connect( _max_contour_distance, SIGNAL(dataChanged( int )), this, SLOT( update_supercell_model_edge_detection_setup() ) );

  ////////////////
  // Super-Cell margin -nm
  ////////////////
  QVector<QVariant> box5_option_1_data_3 = {"Super-cell margin (nm)",""};
  QVector<bool> box5_option_1_3_edit = {false,true};
  boost::function<bool(std::string)> box5_option_1_3_setter ( boost::bind( &TDMap::set_full_boundary_polygon_margin_nm, _core_td_map, _1 ) );
  boost::function<double(void)> box5_option_1_3_getter ( boost::bind( &TDMap::get_full_boundary_polygon_margin_nm, _core_td_map ) );
  TreeItem* super_cell_margin_nm = new TreeItem ( box5_option_1_data_3 , box5_option_1_3_setter, box5_option_1_3_getter, box5_option_1_3_edit );
  super_cell_margin_nm->set_variable_name( "super_cell_margin_nm" );

  edge_detection->insertChildren( super_cell_margin_nm );
  /*group options*/
  super_cell_margin_nm->load_data_from_getter();

  /* validators */
  super_cell_margin_nm->set_flag_validatable_double(1,true);
  boost::function<double(void)> box5_function_1_3_validator_bot ( boost::bind( &TDMap::get_full_boundary_polygon_margin_nm_bottom_limit, _core_td_map ) );
  boost::function<double(void)> box5_function_1_3_validator_top ( boost::bind( &TDMap::get_full_boundary_polygon_margin_nm_top_limit, _core_td_map ) );
  super_cell_margin_nm->set_validator_double_bottom(1, box5_function_1_3_validator_bot );
  super_cell_margin_nm->set_validator_double_top(1, box5_function_1_3_validator_top );

  ui->qtree_view_supercell_model_edge_detection_setup->setModel( super_cell_setup_model );
  ui->qtree_view_supercell_model_edge_detection_setup->setItemDelegate( _load_file_delegate );
  //start editing after one click
  ui->qtree_view_supercell_model_edge_detection_setup->setEditTriggers( QAbstractItemView::AllEditTriggers );
  ui->qtree_view_supercell_model_edge_detection_setup->expandAll();

  for (int column = 0; column < super_cell_setup_model->columnCount(); ++column){
    ui->qtree_view_supercell_model_edge_detection_setup->resizeColumnToContents(column);
  }

}

bool MainWindow::set_dr_probe_path( QString path ){
  _dr_probe_bin_path = path;
  return true;
}

void MainWindow::on_qpush_apply_edge_detection_clicked(){
  bool status = false;
  ui->statusBar->showMessage(tr("Running edge detection"), 2000);
  sim_super_cell_worker->requestSuperCellEdge();
}

void MainWindow::on_qpush_test_tdmap_clicked(){
  const bool project_ok = maybeSetProject();
  if ( project_ok ){
    ui->statusBar->showMessage(tr("Testing if TD Map has a clean run environment"), 2000);
    const bool _warnings_clean = _core_td_map->test_clean_run_env();
    if( _warnings_clean == false ){
      std::vector <std::string> warnings = _core_td_map->get_test_clean_run_env_warnings();
      std::ostringstream os;
      for( int pos = 0; pos < warnings.size(); pos++ ){
        os << warnings.at(pos) << "\n";
      }
      QMessageBox messageBox;
      QFont font;
      font.setBold(false);
      messageBox.setFont(font);
      messageBox.warning(0,"Warning",QString::fromStdString( os.str() ));
    }

    ui->statusBar->showMessage(tr("Testing TD Map variable configuration"), 2000);
    const bool _errors_clean = _core_td_map->test_run_config();
    if( _errors_clean == false ){
      std::vector <std::string> errors = _core_td_map->get_test_run_config_errors();
      std::ostringstream os;
      for( int pos = 0; pos < errors.size(); pos++ ){
        os << errors.at(pos) << "\n";
      }
      QMessageBox messageBox;
      QFont font;
      font.setBold(false);
      messageBox.setFont(font);
      messageBox.critical(0,"Error",QString::fromStdString( os.str() ));
    }
    else{
      QMessageBox messageBox;
      messageBox.information(0,"TD Map ready to run.","The TD Map required variables are setted up. You can run the simulation.");
    }
  }
  else{
    ui->statusBar->showMessage(tr("Error while checking project configurations."), 2000);
  }
}

void MainWindow::on_qpush_run_tdmap_clicked(){
  const bool project_ok = maybeSetProject();
  if ( project_ok ){
    bool status = false;
    updateProgressBar(0,0,4);
    ui->statusBar->showMessage(tr("Requesting a TD-Map worker thread."), 2000);
    clear_tdmap_sim_ostream_containers();
    sim_tdmap_worker->requestTDMap();
  }
  else{
    ui->statusBar->showMessage(tr("Error while checking project configurations."), 2000);
  }
}

void MainWindow::on_qbutton_tdmap_accept_clicked(){
  bool result = false;
  result = _core_td_map->accept_tdmap_best_match_position();
  if( result ){
    ui->statusBar->showMessage(tr("Accepted TD Map best match position."), 2000);
  }
  else{
    ui->statusBar->showMessage(tr("Error while accepting TD Map best match position.") );
  }
}
