// A simple program that computes the square root of a number
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <iostream>
#include <string>


#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv_modules.hpp"
#include "opencv2/features2d/features2d.hpp"

// Include the headers relevant to the boost::program_options
// library
#include <boost/regex.hpp>
#include "boost/tokenizer.hpp"
//#include <boost/program_options/variables_map.hpp>
#define _HAS_AUTO_PTR_ETC 1
#include "boost/token_functions.hpp"
#define _HAS_AUTO_PTR_ETC 1
#include <boost/program_options.hpp>
#define _HAS_AUTO_PTR_ETC 1

#include <boost/program_options/parsers.hpp>
#define _HAS_AUTO_PTR_ETC 1
#include <boost/program_options/variables_map.hpp>
#include <boost/tokenizer.hpp>
#include <boost/token_functions.hpp>

#include "unistd.h"

int main (int argc, char *argv[])
{
    cv::Mat experimental_image(2,3,CV_8UC1);
    std::cout << "win 32 test" << experimental_image << std::endl;


	std::string line;
	boost::regex pat("^Subject: (Re: |Aw: )*(.*)");

	while (std::cin)
	{
		std::getline(std::cin, line);
		boost::smatch matches;
		if (boost::regex_match(line, matches, pat))
			std::cout << matches[2] << std::endl;
	}


    return 0;
}
