#include "super_cell.hpp"

SuperCell::SuperCell(){
}

SuperCell::SuperCell( UnitCell* cell ){
  unit_cell = cell;
  _flag_unit_cell = true;
}

/* Loggers */
bool SuperCell::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
  ImageBounds::set_application_logger( app_logger );
  BaseImage::set_application_logger( app_logger );
  logger->logEvent( ApplicationLog::notification, "Application logger setted for SuperCell class." );
  return true;
}

void SuperCell::print_var_state(){
  if( _flag_logger ){
    std::stringstream message;
    output( message );
    logger->logEvent( ApplicationLog::notification , message.str() );
  }
}

std::ostream& operator<<(std::ostream& stream, const SuperCell& var) {
  var.output(stream);
  return stream;
}

std::ostream& SuperCell::output(std::ostream& stream) const {
  stream << "SuperCell vars:\n" <<
  "\t" << "ImageBounds Properties : " << "\n";
  ImageBounds::output(stream);
  stream << "BaseImage Properties : " << "\n";
  BaseImage::output(stream);
  return stream;
}
