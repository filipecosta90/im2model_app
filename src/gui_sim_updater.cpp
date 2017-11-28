#include "gui_sim_updater.hpp"

GuiSimOutUpdater::GuiSimOutUpdater(TDMap* core_tdmap ,  QObject *parent ) : QObject(parent) {
  _working =false;
  _abort = false;
  tdmap = core_tdmap;
  qDebug()<<"creating GuiSimOutUpdater in Thread "<<thread()->currentThreadId();

}

void GuiSimOutUpdater::newSuperCellFull_intensity_cols(){
  qDebug()<<"Starting worker process in Thread "<<thread()->currentThreadId();

  bool result = tdmap->compute_full_super_cell_intensity_cols();
  if ( result ){
    std::cout << "emiting sucess" << std::endl;
    emit SuperCell_full_intensity_cols_sucess();
  }
  else {
    std::cout << "emiting failure" << std::endl;
    emit SuperCell_full_intensity_cols_failure();
  }
  // Set _working to false, meaning the process can't be aborted anymore.
  mutex.lock();
  _working = false;
  mutex.unlock();
  std::cout << "emiting finished" << std::endl;
  emit SuperCell_full_intensity_cols_finished();
}

void GuiSimOutUpdater::newSuperCellFull(){
  qDebug()<<"Starting worker process in Thread "<<thread()->currentThreadId();

  bool result = tdmap->compute_full_super_cell();
  if ( result ){
    std::cout << "emiting sucess" << std::endl;
    emit SuperCell_full_sucess();
  }
  else {
    std::cout << "emiting failure" << std::endl;
    emit SuperCell_full_failure();
  }
  // Set _working to false, meaning the process can't be aborted anymore.
  mutex.lock();
  _working = false;
  mutex.unlock();
  std::cout << "emiting finished" << std::endl;
  emit SuperCell_full_finished();
}

void   GuiSimOutUpdater::newSuperCellEdge() {
  qDebug()<<"Starting worker process in Thread "<<thread()->currentThreadId();

  bool result = tdmap->calculate_exp_image_boundaries_from_full_image();
  if ( result ){
    std::cout << "emiting sucess" << std::endl;
    emit SuperCell_edge_sucess();
  }
  else {
    std::cout << "emiting failure" << std::endl;
    emit SuperCell_edge_failure();
  }
  // Set _working to false, meaning the process can't be aborted anymore.
  mutex.lock();
  _working = false;
  mutex.unlock();
  std::cout << "emiting finished" << std::endl;
  emit SuperCell_edge_finished();
}

void   GuiSimOutUpdater::newTDMapSim() {
  qDebug()<<"Starting worker process in Thread "<<thread()->currentThreadId();

  const bool _vars_setted_up = tdmap->test_run_config();
  if ( _vars_setted_up ) {
    emit  TDMap_started();
    bool result = tdmap->run_tdmap();
    if ( result ){
      if( tdmap->get_run_simgrid_switch() ){
        emit TDMap_sucess();
      }
      else{
        emit TDMap_sucess_no_correlation();
      }
    }
  }
  else {
    emit TDMap_failure();
  }

  // Set _working to false, meaning the process can't be aborted anymore.
  mutex.lock();
  _working = false;
  mutex.unlock();
  emit TDMap_finished();

}

void GuiSimOutUpdater::requestTDMap(){
  qDebug()<<"going to lock mutex "<<thread()->currentThreadId();
  mutex.lock();
  _working = true;
  _abort = false;
  mutex.unlock();
  qDebug()<<"Request worker start in Thread "<<thread()->currentThreadId();
  emit TDMap_request();
}

void GuiSimOutUpdater::requestSuperCellEdge(){
  qDebug()<<"going to lock mutex "<<thread()->currentThreadId();
  mutex.lock();
  _working = true;
  _abort = false;
  mutex.unlock();
  qDebug()<<"Request worker start in Thread "<<thread()->currentThreadId();
  emit SuperCell_edge_request();
  qDebug()<<"unlocked mutex "<<thread()->currentThreadId();
}

void GuiSimOutUpdater::requestFullSuperCell(){
  qDebug()<<"going to lock mutex "<<thread()->currentThreadId();
  mutex.lock();
  _working = true;
  _abort = false;
  mutex.unlock();
  qDebug()<<">>Request full super cell worker start in Thread "<<thread()->currentThreadId();
  emit SuperCell_full_request();
  qDebug()<<"unlocked mutex "<<thread()->currentThreadId();
}


void GuiSimOutUpdater::requestFullSuperCellComputeIntensityCols(){
  qDebug()<<"going to lock mutex "<<thread()->currentThreadId();
  mutex.lock();
  _working = true;
  _abort = false;
  mutex.unlock();
  qDebug()<<">>Request full super cell worker to compute intensity cols to start in Thread "<<thread()->currentThreadId();
  emit SuperCell_full_intensity_cols_request();
  qDebug()<<"unlocked mutex "<<thread()->currentThreadId();
}

void GuiSimOutUpdater::abort(){
  mutex.lock();
  if (_working) {
    _abort = true;
    qDebug()<<"Request worker aborting in Thread "<<thread()->currentThreadId();
  }
  mutex.unlock();
}
