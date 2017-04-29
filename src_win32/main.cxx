// A simple program that computes the square root of a number
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <iostream>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv_modules.hpp"
#include "opencv2/features2d/features2d.hpp"

// Include the headers relevant to the boost::program_options
// library
//#include "boost/program_options.hpp"
//#include <boost/program_options/parsers.hpp>
//#include <boost/program_options/variables_map.hpp>
//#include <boost/tokenizer.hpp>
//#include <boost/token_functions.hpp>

int main (int argc, char *argv[])
{
    cv::Mat experimental_image(2,3,CV_8UC1);
    std::cout << "win 32 test" << experimental_image << std::endl;

    return 0;
}
