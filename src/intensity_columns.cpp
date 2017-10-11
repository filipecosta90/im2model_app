#include "intensity_columns.hpp"

IntensityColumns::IntensityColumns( ){
}

bool IntensityColumns::set_wavimg_var( WAVIMG_prm *wavimg_var ){
  wavimg_parameters = wavimg_var;
  return true;
}

bool IntensityColumns::set_sim_crystal_properties ( BaseCrystal* crystal_prop ){
  sim_crystal_properties = crystal_prop;
  _flag_sim_crystal_properties = true;
  return true;
}

bool IntensityColumns::set_exp_image_properties ( BaseImage* exp_prop ){
  exp_image_properties = exp_prop;
  _flag_exp_image_properties = true;
  return true;
}

bool IntensityColumns::set_sim_image_properties ( BaseImage* sim_prop ){
  sim_image_properties = sim_prop;
  _flag_sim_image_properties = true;
  return true;
}

bool IntensityColumns::read_simulated_image_from_dat_file(){
  bool result = false;
  if( _flag_sim_crystal_properties ){
    if(
        // BaseCrystal vars
        sim_crystal_properties->get_flag_base_dir_path() &&
        sim_crystal_properties->get_flag_dat_output_target_folder() &&
        // BaseImage vars
        sim_image_properties->get_flag_full_n_rows_height() &&
        sim_image_properties->get_flag_full_n_cols_width()
      ){
      // get const vars from class pointer
      const boost::filesystem::path base_dir_path = sim_crystal_properties->get_base_dir_path();
      const int default_full_n_rows_height = sim_image_properties->get_full_n_rows_height();
      const int default_full_n_cols_width = sim_image_properties->get_full_n_cols_width();
      const std::string dat_output_target_folder = sim_crystal_properties->get_dat_output_target_folder();
      const std::string file_name_output_image_wave_function = wavimg_parameters->get_file_name_output_image_wave_function();
      boost::filesystem::path dat_input_dir ( dat_output_target_folder );

      std::stringstream output_dat_name_stream;
      output_dat_name_stream << file_name_output_image_wave_function << ".dat";
      boost::filesystem::path dat_file ( output_dat_name_stream.str() );
      boost::filesystem::path full_dat_path = base_dir_path / dat_input_dir / dat_file;
      int full_n_rows_height = default_full_n_rows_height;
      int full_n_cols_width = default_full_n_cols_width;
      bool _mmap_ok = false;
      float* p;
      try {
        boost::iostreams::mapped_file_source mmap( full_dat_path );
        _mmap_ok = true;
        p = (float*) mmap.data();
        cv::Mat raw_simulated_image ( full_n_rows_height , full_n_cols_width , CV_32FC1);
        int pos = 0;
        float *pixel;
        for (int row = 0; row < full_n_rows_height; row++) {
          for (int col = 0; col < full_n_cols_width; col++) {
            raw_simulated_image.at<float>(row, full_n_cols_width - col -1) = (float) p[pos] ;
            pos++;
          }
        }
        mmap.close();
        const bool set_result = sim_image_properties->set_full_image( raw_simulated_image );
        if( set_result ){
          imwrite("intensity_columns_base.png", raw_simulated_image);
          result = true;
        }
      }
      catch(const std::ios_base::failure & e) {
        _mmap_ok = false;
        if( _flag_logger ){
          std::stringstream message;
          message << "Caught std::ios_base::failure: " << typeid(e).name();
          BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
        }
      }
      catch(const std::exception & e) {
        _mmap_ok = false;
        if( _flag_logger ){
          std::stringstream message;
          message << "Caught std::exception: " << typeid(e).name();
          BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
        }
      }
    }
    else {
      result = false;
      if( _flag_logger ){
        std::stringstream message;
        message << "The required vars for read_image_from_dat_file() are not setted up.";
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
      }
      print_var_state();
    }
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The required Class POINTERS for read_image_from_dat_file() are not setted up.";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
  if( _flag_logger ){
    std::stringstream message;
    message << "Overall read_image_from_dat_file result: " << std::boolalpha  << result;
    if( result ){
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
    }
    else{
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
  }
  return result;
}

/* Loggers */
bool IntensityColumns::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
  BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification, "Application logger setted for IntensityColumns class." );
  return true;
}

void IntensityColumns::print_var_state(){
  if( _flag_logger ){
    std::stringstream message;
    output(message);
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
  }
}

std::ostream& IntensityColumns::output(std::ostream& stream) const {
  stream << "IntensityColumns vars:\n";
  // simulated images

  return stream;
}
