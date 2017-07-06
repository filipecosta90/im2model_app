#include "emd_wrapper.h"

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
      EMDGroup grp(grp_id);
      grp.scan_group( grp_id );
      status = H5Fclose(file->getId());
      const bool data_flag = grp.get_flag_contains_group("/Data");
      if( data_flag ){
        std::cout << std::boolalpha << "grp.get_flag_contains_group(/Data)" << grp.get_flag_contains_group("/Data") << std::endl;

        EMDGroup* grp_1 = grp.get_group("/Data");
        EMDGroup* grp_2 = grp_1->get_group("/Data/Image");
        EMDGroup* grp_3 = grp_2->get_child_group(0);
        EMDDataSet* ds_grp_3 = grp_3->get_dataset(grp_3->get_name()+"/Data");
        std::vector<unsigned char> data_ds_grp_3 = ds_grp_3->get_raw_data();
        std::vector<unsigned short>    b(data_ds_grp_3.size()/2);
        memcpy(&b[0], &data_ds_grp_3[0], data_ds_grp_3.size());

        EMDDataSet* ds_grp_3_meta = grp_3->get_dataset(grp_3->get_name()+"/Metadata");
        std::vector<unsigned char> metadata_ds_grp_3 = ds_grp_3_meta->get_raw_data();
        std::vector<char>    meta(metadata_ds_grp_3.size()/2);
        memcpy(&meta[0], &metadata_ds_grp_3[0], metadata_ds_grp_3.size());

        // Read json.

        rapidjson::StringStream metadata_stream ( std::string(meta.begin(), meta.end()).c_str() );
        //std::cout << "'" << metadata_stream.str() << "'";

        try{
          Document metadata_doc;
          metadata_doc.Parse(std::string(meta.begin(), meta.end()).c_str());
          std::cout << "metadata_doc[\"Optics\"][\"AccelerationVoltage\"] :\t" << metadata_doc["Optics"]["AccelerationVoltage"].GetString() << std::endl;
          std::cout << "metadata_doc[\"BinaryResult\"][\"PixelSize\"][\"width\"] :\t" << metadata_doc["BinaryResult"]["PixelSize"]["width"].GetString() << std::endl;
          std::cout << "metadata_doc[\"BinaryResult\"][\"PixelSize\"][\"height\"] :\t" << metadata_doc["BinaryResult"]["PixelSize"]["height"].GetString() << std::endl;

        }
        catch (std::exception const& e)
        {
          std::cerr << " ERROR: " << e.what() << std::endl;
        }

        hsize_t* dims = ds_grp_3->get_chunk_dims_out();

        int n_rows = dims[0];
        int n_cols = dims[1];
        cv::Mat raw_simulated_image ( n_rows , n_cols , CV_16UC1);
        int pos = 0;
        for (int row = 0; row < n_rows; row++) {
          for (int col = 0; col < n_cols; col++) {
            raw_simulated_image.at<unsigned short>(row, col) = b.at(row * n_rows + col ) ;
            pos++;
          }
        }
        imwrite("test.png",raw_simulated_image);
        result = true;
      }
    } catch(const H5::FileIException&) {
    }
  }
  return result;
}
