#include "emd_wrapper.h"

EMDWrapper::EMDWrapper(){}

EMDWrapper::~EMDWrapper(){
  full_image.release();
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
        std::vector<unsigned char> full_image_data = ds_grp_3->get_raw_data();
        hsize_t* dims = ds_grp_3->get_chunk_dims_out();

        int n_rows = dims[0];
        int n_cols = dims[1];
        full_image = cv::Mat ( n_rows , n_cols , cv::DataType<unsigned short>::type , full_image_data.data() );

        EMDDataSet* ds_grp_3_meta = grp_3->get_dataset(grp_3->get_name()+"/Metadata");
        const std::vector<unsigned char> metadata_ds_grp_3 = ds_grp_3_meta->get_raw_data();
        std::vector<char> meta( metadata_ds_grp_3.size() );
        memcpy( &meta[0], metadata_ds_grp_3.data(), metadata_ds_grp_3.size() );

        // Read json.
        Document metadata_doc;
        metadata_doc.Parse( std::string(meta.begin(), meta.end()).c_str() );
        Optics_AccelerationVoltage = boost::lexical_cast<double>( metadata_doc["Optics"]["AccelerationVoltage"].GetString() );
        BinaryResult_PixelSize_width = boost::lexical_cast<double>( metadata_doc["BinaryResult"]["PixelSize"]["width"].GetString() );
        BinaryResult_PixelSize_height = boost::lexical_cast<double>( metadata_doc["BinaryResult"]["PixelSize"]["height"].GetString() );

        std::cout << "( " << Optics_AccelerationVoltage << " ) metadata_doc[\"Optics\"][\"AccelerationVoltage\"] :\t" << metadata_doc["Optics"]["AccelerationVoltage"].GetString() << std::endl;
        std::cout << "( " << BinaryResult_PixelSize_width << " ) metadata_doc[\"BinaryResult\"][\"PixelSize\"][\"width\"] :\t" << metadata_doc["BinaryResult"]["PixelSize"]["width"].GetString() << std::endl;
        std::cout << "( " << BinaryResult_PixelSize_height << " ) metadata_doc[\"BinaryResult\"][\"PixelSize\"][\"height\"] :\t" << metadata_doc["BinaryResult"]["PixelSize"]["height"].GetString() << std::endl;
        result = true;
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
