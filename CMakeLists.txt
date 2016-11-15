# the minimum version of CMake required
cmake_minimum_required(VERSION 2.8)

project( im2model )
# need to improve this input 

set(Boost_USE_STATIC_LIBS ON) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 

SET(BOOST_ROOT /usr/local/Cellar/boost/1.62.0)
SET(BOOST_INCLUDEDIR /usr/local/Cellar/boost/1.62.0/include)
SET(BOOST_LIBRARYDIR /usr/local/Cellar/boost/1.62.0/lib)

find_package(Boost COMPONENTS program_options REQUIRED)

include_directories(${BOOST_INCLUDE_DIRS})

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# we add the files in the include directory to be included
# while compiling all the source code
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# we add the sub-directories that we want CMake to scan
add_subdirectory(src)


