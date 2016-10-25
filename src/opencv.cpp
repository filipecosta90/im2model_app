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

using namespace cv;

int main(int argc, char** argv )
{
  if ( argc != 4 )
  {
    printf("usage: DisplayImage.out <Image_Path>\n");
    return -1;
  }

  int fd;
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
  cv::Mat image ( cols, rows , CV_8SC1);
  int pos = 0;
  for (int col = 0; col < cols; col++) {
    for (int row = 0; row < rows; row++) {
      image.at<uchar>(col, row) = (uchar) ( ( p[pos] / 256.0f ) ); 
      pos++;
    }
  }

  if ( !image.data )
  {
    printf("No image data \n");
    return -1;
  }
  namedWindow("Display Image", WINDOW_AUTOSIZE );
  imshow("Display Image", image);

  waitKey(0);

  return 0;
}
