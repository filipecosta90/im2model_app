#include <cctype>
#include <fstream>
#include <cassert>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "msa_prm.h"
#include "wavimg_prm.h"

using namespace cv;

// Global Variables
int match_method;
int max_Trackbar = 5;

cv::Mat img;
cv::Mat templ; 
cv::Mat result;

char* image_window = "Source Image";
char* result_window = "Result window";

/// Function Headers
void MatchingMethod( int, void* );

int main(int argc, char** argv )
{
  if ( argc != 5 )
  {
    printf("usage: im2model <Simulated_Image_Path> <Width> <Height> <Experimental_Image_Path> \n");
    return -1;
  }

  MSA_prm::MSA_prm msa_parameters;
  msa_parameters.set_electron_wavelength( 0.00196875 );
  msa_parameters.set_internal_repeat_factor_of_super_cell_along_x ( 1 );
  msa_parameters.set_internal_repeat_factor_of_super_cell_along_y ( 1 );
  msa_parameters.set_slice_filename_prefix ( "'test'" );
  msa_parameters.set_number_slices_to_load ( 119 );
  msa_parameters.set_number_frozen_lattice_variants_considered_per_slice( 1 );
  msa_parameters.set_minimum_number_frozen_phonon_configurations_used_generate_wave_functions ( 1 );
  msa_parameters.set_period_readout_or_detection_in_units_of_slices ( 8 );
  msa_parameters.set_number_slices_used_describe_full_object_structure_up_to_its_maximum_thickness ( 119 );
  msa_parameters.set_linear_slices_for_full_object_structure();
  msa_parameters.produce_prm("msa_119_im2model.prm");
  int fd;
  WAVIMG_prm::WAVIMG_prm wavimg_parameters;


  fd = open ( argv[1], O_RDONLY );
  if ( fd == -1 ){
    perror("open");
  }

  off_t fsize;
  fsize = lseek(fd, 0, SEEK_END);
  int* p;
  std::cout << "Total file size in bytes " << fsize << std::endl;    

  p = (int*) mmap (0, fsize, PROT_READ, MAP_SHARED, fd, 0);                  

  if (p == MAP_FAILED) {                                                        
    perror ("mmap");                                                            
  }                                                                             

  if (close (fd) == -1) {                                                       
    perror ("close");                                                           
  }                                                                             

  int rows = atoi(argv[2]);
  int cols = atoi(argv[3]);

  // Load image and template
  cv::Mat image ( cols, rows , CV_8SC1);
  int pos = 0;
  for (int col = 0; col < cols; col++) {
    for (int row = 0; row < rows; row++) {
      image.at<char>(col, row) =  ( ( p[pos] / 256.0f ) ); 
      pos++;
    }
  }

  // Create windows
  templ = imread( argv[4], 1 );

  if ( !image.data || !templ.data)
  {
    printf("No image data \n");
    return -1;
  }

  namedWindow(argv[1], WINDOW_AUTOSIZE );
  imshow(argv[1], image);
  namedWindow( result_window, CV_WINDOW_AUTOSIZE );


  /// Create Trackbar
  char* trackbar_label = "Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED";
  createTrackbar( trackbar_label, image_window, &match_method, max_Trackbar, MatchingMethod );
  waitKey(0);

  return 0;
}

/**
 * @function MatchingMethod
 * @brief Trackbar callback
 */
void MatchingMethod( int, void* )
{
  /// Source image to display
  Mat img_display;
  img.copyTo( img_display );

  /// Create the result matrix
  int result_cols =  img.cols - templ.cols + 1;
  int result_rows = img.rows - templ.rows + 1;

  result.create( result_rows, result_cols, CV_32FC1 );

  /// Do the Matching and Normalize
  matchTemplate( img, templ, result, match_method );
  normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );

  /// Localizing the best match with minMaxLoc
  double minVal; double maxVal; Point minLoc; Point maxLoc;
  Point matchLoc;

  minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );

  /// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
  if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED )
  { matchLoc = minLoc; }
  else
  { matchLoc = maxLoc; }

  /// Show me what you got
  rectangle( img_display, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );
  rectangle( result, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );

  imshow( image_window, img_display );
  imshow( result_window, result );

  return;
}
