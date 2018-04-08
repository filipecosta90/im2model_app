/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#include "emd_wrapper.h"
using namespace H5;

EMDWrapper::EMDWrapper(){}

EMDWrapper::~EMDWrapper(){
  //full_image.release();
}


std::string EMDWrapper::type2str(int type){
  std::string r;

  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (type >> CV_CN_SHIFT);

  switch ( depth ) {
    case CV_8U:  r = "8U"; break;
    case CV_8S:  r = "8S"; break;
    case CV_16U: r = "16U"; break;
    case CV_16S: r = "16S"; break;
    case CV_32S: r = "32S"; break;
    case CV_32F: r = "32F"; break;
    case CV_64F: r = "64F"; break;
    default:     r = "User"; break;
  }

  r += "C";
  r += (chans+'0');

  return r;
}

bool EMDWrapper::read_emd( std::string filename ){
  bool result = false;

  bool valid_file = H5::H5File::isHdf5( filename.c_str() );
  if (valid_file ){
    try {
      hid_t    grp_id;
      herr_t   status;
      /*
       *  Example: open a file, open the root, scan the whole file.
       */
      H5File *file = new H5File( filename.c_str(), H5F_ACC_RDWR );
      grp_id = H5Gopen(file->getId(),"/", H5P_DEFAULT);
      grp.scan_group( grp_id );
      status = H5Fclose(file->getId());
      const bool data_flag = grp.get_flag_contains_group("/Data");
      if( data_flag ){
        std::cout << std::boolalpha << "grp.get_flag_contains_group(/Data)" << grp.get_flag_contains_group("/Data") << std::endl;
        EMDGroup* grp_1 = grp.get_group("/Data");
        EMDGroup* grp_2 = grp_1->get_group("/Data/Image");
        EMDGroup* grp_3 = grp_2->get_child_group(0);
        EMDDataSet* ds_grp_3 = grp_3->get_dataset(grp_3->get_name()+"/Data");

        std::vector<unsigned char>* full_image_data = ds_grp_3->get_raw_data();
        const bool full_image_data_is_type_numeric = ds_grp_3->is_numeric();
        bool error_flag = false;
        if( full_image_data_is_type_numeric ){

          const hid_t native_dtype  = ds_grp_3->get_native_dtype();  
          int mat_type;
          if( native_dtype == H5T_NATIVE_CHAR ){
            mat_type = cv::DataType<char>::type;
          }
          else if( native_dtype == H5T_NATIVE_SHORT ){
            mat_type = cv::DataType<short>::type;
          }
          else if( native_dtype == H5T_NATIVE_INT ){
            mat_type = cv::DataType<int>::type;
          }
          else if( native_dtype == H5T_NATIVE_LONG ){
            mat_type = cv::DataType<long>::type;
          }
          else if( native_dtype == H5T_NATIVE_UCHAR ){
            mat_type = cv::DataType<unsigned char>::type;
          }
          else if( native_dtype == H5T_NATIVE_USHORT ){
            mat_type = cv::DataType<unsigned short>::type;
          }
          else if( native_dtype == H5T_NATIVE_UINT){
            mat_type = cv::DataType<unsigned int>::type;
          }
          else if( native_dtype == H5T_NATIVE_ULONG){
            mat_type = cv::DataType<unsigned long>::type;
          }
          else if( native_dtype == H5T_NATIVE_FLOAT){
            mat_type = cv::DataType<float>::type;
          }
          else if( native_dtype == H5T_NATIVE_DOUBLE){
            mat_type = cv::DataType<double>::type;
          }
          else{
            error_flag = true;
          }
          if ( error_flag == false ){
            hsize_t* dims = ds_grp_3->get_chunk_dims_out();
            const hsize_t n_rows = dims[0];
            const hsize_t n_cols = dims[1];
            std::cout << " n_rows " << n_rows <<" n_cols " << n_cols << std::endl;
            std::cout << " type " << type2str( mat_type )  << std::endl;

            full_image = cv::Mat ( n_rows , n_cols , mat_type , full_image_data->data() );

            EMDDataSet* ds_grp_3_meta = grp_3->get_dataset(grp_3->get_name()+"/Metadata");
            std::vector<unsigned char>* metadata_ds_grp_3 = ds_grp_3_meta->get_raw_data();
            std::vector<char> meta( metadata_ds_grp_3->size() );
            memcpy( &meta[0], metadata_ds_grp_3->data(), metadata_ds_grp_3->size() );

          // Read json.
            Document metadata_doc;
            metadata_doc.Parse( std::string(meta.begin(), meta.end()).c_str() );
            Optics_AccelerationVoltage = boost::lexical_cast<double>( metadata_doc["Optics"]["AccelerationVoltage"].GetString() );
            _flag_Optics_AccelerationVoltage = true;
            BinaryResult_PixelSize_width = boost::lexical_cast<double>( metadata_doc["BinaryResult"]["PixelSize"]["width"].GetString() );
            _flag_BinaryResult_PixelSize_width = true;
            BinaryResult_PixelSize_height = boost::lexical_cast<double>( metadata_doc["BinaryResult"]["PixelSize"]["height"].GetString() );
            _flag_BinaryResult_PixelSize_height = true;
            std::cout << "( " << Optics_AccelerationVoltage << " ) metadata_doc[\"Optics\"][\"AccelerationVoltage\"] :\t" << metadata_doc["Optics"]["AccelerationVoltage"].GetString() << std::endl;
            std::cout << "( " << BinaryResult_PixelSize_width << " ) metadata_doc[\"BinaryResult\"][\"PixelSize\"][\"width\"] :\t" << metadata_doc["BinaryResult"]["PixelSize"]["width"].GetString() << std::endl;
            std::cout << "( " << BinaryResult_PixelSize_height << " ) metadata_doc[\"BinaryResult\"][\"PixelSize\"][\"height\"] :\t" << metadata_doc["BinaryResult"]["PixelSize"]["height"].GetString() << std::endl;
            result = true;
          }
          std::cout << "out of emd wrapper, result " <<  std::boolalpha << result <<  std::endl;
        }
        // datatype not supported by im2model
        else{

        }

      }
    } catch(const H5::FileIException& e) {
      std::cerr << " ERROR FileIException: "<< std::endl;
    }
    catch(boost::bad_lexical_cast&  ex) {
      // pass it up
      boost::throw_exception( ex );
    }
    catch (std::exception const& e)
    {
      std::cerr << " ERROR: " << e.what() << std::endl;
    }
  }
  return result;
}

cv::Mat EMDWrapper::get_full_image(){
  return full_image.clone();
}

//dimensions
int EMDWrapper::get_full_n_rows_height(){
  return full_image.rows;
}

int EMDWrapper::get_full_n_cols_width(){
  return full_image.cols;
}
