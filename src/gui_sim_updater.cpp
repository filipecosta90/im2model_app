/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#include "gui_sim_updater.hpp"

GuiSimOutUpdater::GuiSimOutUpdater(TDMap* core_tdmap ,  QObject *parent ) : QObject(parent) {
  _working =false;
  _abort = false;
  tdmap = core_tdmap;
  qDebug()<<"creating GuiSimOutUpdater in Thread "<<thread()->currentThreadId();

}

/* Loggers */
bool GuiSimOutUpdater::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
  BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification, "Application logger setted for GuiSimOutUpdater class." );
  return true;
}

void GuiSimOutUpdater::newSuperCellFull_intensity_cols(){

  if( _flag_logger ){
    std::stringstream message;
    message << "Starting worker process in Thread " <<thread()->currentThreadId();
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
  }

  bool result = tdmap->compute_full_super_cell_intensity_cols();
  if ( result ){

    if( _flag_logger ){
      std::stringstream message;
      message << "Emitting sucess on compute_full_super_cell_intensity_cols. emit SuperCell_full_intensity_cols_sucess()";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
    }
    emit SuperCell_full_intensity_cols_sucess();
  }
  else {
    if( _flag_logger ){
      std::stringstream message;
      message << "Emitting failure on compute_full_super_cell_intensity_cols. emit SuperCell_full_intensity_cols_failure()";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
    emit SuperCell_full_intensity_cols_failure();
  }
  // Set _working to false, meaning the process can't be aborted anymore.
  mutex.lock();
  _working = false;
  mutex.unlock();
  if( _flag_logger ){
    std::stringstream message;
    message << "Emitting finished on compute_full_super_cell_intensity_cols. emit SuperCell_full_intensity_cols_finished()";
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
  }
  emit SuperCell_full_intensity_cols_finished();
}

void GuiSimOutUpdater::newSuperCellFull(){
  if( _flag_logger ){
    std::stringstream message;
    message << "Starting worker process in Thread " <<thread()->currentThreadId();
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
  }

  bool result = tdmap->compute_full_super_cell();
  if ( result ){
    if( _flag_logger ){
      std::stringstream message;
      message << "Emitting sucess on compute_full_super_cell. emit SuperCell_full_sucess()";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
    }
    emit SuperCell_full_sucess();
  }
  else {
    if( _flag_logger ){
      std::stringstream message;
      message << "Emitting failure on compute_full_super_cell. emit SuperCell_full_failure()";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
    emit SuperCell_full_failure();
  }
  // Set _working to false, meaning the process can't be aborted anymore.
  mutex.lock();
  _working = false;
  mutex.unlock();
  if( _flag_logger ){
    std::stringstream message;
    message << "Emitting finished on compute_full_super_cell. emit SuperCell_full_finished()";
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
  }
  emit SuperCell_full_finished();
}

void   GuiSimOutUpdater::newSuperCellEdge() {
  if( _flag_logger ){
    std::stringstream message;
    message << "Starting worker (newSuperCellEdge) process in Thread " <<thread()->currentThreadId();
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
  }

  bool result = tdmap->calculate_exp_image_boundaries_from_full_image();
  if ( result ){
    if( _flag_logger ){
      std::stringstream message;
      message << "Emitting sucess on newSuperCellEdge. emit SuperCell_edge_sucess()";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
    }
    emit SuperCell_edge_sucess();
  }
  else {
    if( _flag_logger ){
      std::stringstream message;
      message << "Emitting failure on newSuperCellEdge. emit SuperCell_edge_failure()";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
    emit SuperCell_edge_failure();
  }
  // Set _working to false, meaning the process can't be aborted anymore.
  mutex.lock();
  _working = false;
  mutex.unlock();
  if( _flag_logger ){
    std::stringstream message;
    message << "Emitting finished on newSuperCellEdge. emit SuperCell_edge_finished()";
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
  }
  emit SuperCell_edge_finished();
}

void   GuiSimOutUpdater::newTDMapSim() {
 if( _flag_logger ){
  std::stringstream message;
  message << "Starting worker ( newTDMapSim ) process in Thread " <<thread()->currentThreadId();
  BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
}

const bool _vars_setted_up = tdmap->test_run_config();
if ( _vars_setted_up ) {
  emit  TDMap_started();
  bool result = tdmap->run_tdmap();
  if ( result ){
    if( tdmap->get_run_simgrid_switch() ){
      if( _flag_logger ){
        std::stringstream message;
        message << "Emitting sucess on newTDMapSim. emit TDMap_sucess()";
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
      }
      emit TDMap_sucess();
    }
    else{
      if( _flag_logger ){
        std::stringstream message;
        message << "Emitting sucess on newTDMapSim. emit TDMap_sucess_no_correlation()";
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
      }
      emit TDMap_sucess_no_correlation();
    }
  }
}
else {
  if( _flag_logger ){
    std::stringstream message;
    message << "Emitting failure on newTDMapSim. emit TDMap_failure()";
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
  }
  emit TDMap_failure();
}

  // Set _working to false, meaning the process can't be aborted anymore.
mutex.lock();
_working = false;
mutex.unlock();
if( _flag_logger ){
  std::stringstream message;
  message << "Emitting finished on newTDMapSim. emit TDMap_finished()";
  BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
}
emit TDMap_finished();

}

void GuiSimOutUpdater::requestTDMap(){
  if( _flag_logger ){
    std::stringstream message;
    message << "requestTDMap(). going to lock mutex "<<thread()->currentThreadId();
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
  }
  mutex.lock();
  _working = true;
  _abort = false;
  mutex.unlock();
  if( _flag_logger ){
    std::stringstream message;
    message << "requestTDMap(). Request worker start in Thread "<<thread()->currentThreadId();
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
  }
  emit TDMap_request();
  if( _flag_logger ){
    std::stringstream message;
    message << "requestTDMap(). unlocked mutex on "<<thread()->currentThreadId();
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
  }
}

void GuiSimOutUpdater::requestSuperCellEdge(){
  if( _flag_logger ){
    std::stringstream message;
    message << "requestSuperCellEdge(). going to lock mutex "<<thread()->currentThreadId();
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
  }
  mutex.lock();
  _working = true;
  _abort = false;
  mutex.unlock();
  if( _flag_logger ){
    std::stringstream message;
    message << "requestSuperCellEdge(). Request worker start in Thread "<<thread()->currentThreadId();
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
  }
  emit SuperCell_edge_request();
  if( _flag_logger ){
    std::stringstream message;
    message << "requestSuperCellEdge(). unlocked mutex on "<<thread()->currentThreadId();
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
  }
}

void GuiSimOutUpdater::requestFullSuperCell(){
  if( _flag_logger ){
    std::stringstream message;
    message << "requestFullSuperCell(). going to lock mutex "<<thread()->currentThreadId();
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
  }
  mutex.lock();
  _working = true;
  _abort = false;
  mutex.unlock();
  if( _flag_logger ){
    std::stringstream message;
    message << "requestFullSuperCell(). Request worker start in Thread "<<thread()->currentThreadId();
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
  }
  emit SuperCell_full_request();
  if( _flag_logger ){
    std::stringstream message;
    message << "requestFullSuperCell(). unlocked mutex on "<<thread()->currentThreadId();
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
  }
}


void GuiSimOutUpdater::requestFullSuperCellComputeIntensityCols(){
 if( _flag_logger ){
  std::stringstream message;
  message << "requestFullSuperCellComputeIntensityCols(). going to lock mutex "<<thread()->currentThreadId();
  BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
}
mutex.lock();
_working = true;
_abort = false;
mutex.unlock();
if( _flag_logger ){
  std::stringstream message;
  message << "requestFullSuperCellComputeIntensityCols(). Request worker start in Thread "<<thread()->currentThreadId();
  BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
}
emit SuperCell_full_intensity_cols_request();
if( _flag_logger ){
  std::stringstream message;
  message << "requestFullSuperCellComputeIntensityCols(). unlocked mutex on "<<thread()->currentThreadId();
  BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
}
}

void GuiSimOutUpdater::abort(){
  mutex.lock();
  if (_working) {
    _abort = true;

    if( _flag_logger ){
      std::stringstream message;
      message << "GuiSimOutUpdater. Request worker aborting in Thread "<<thread()->currentThreadId();
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
    }
  }
  mutex.unlock();
}
