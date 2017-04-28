
#include <cctype>
#include <fstream>
#include <cassert>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <cstdio>
#include <sstream>      // std::stringstream
#include <string>       // std::string
#include <iostream>     // std::cout
#include <iomanip>
#include <vector>
#include <stdio.h>

#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "opencv2/opencv_modules.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"

#include "atom.hpp"


Atom::Atom()
{
}

