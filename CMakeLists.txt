# the minimum version of CMake required
cmake_minimum_required(VERSION 3.6.2)
set (CMAKE_CXX_COMPILER "clang++" )
set (CMAKE_CXX_STANDARD 14)

project( MSC_IM2MODEL )

# Turn on the ability to create folders to organize projects (.vcproj)
# It creates "CMakePredefinedTargets" folder by default and adds CMake
# defined projects like INSTALL.vcproj and ZERO_CHECK.vcproj
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(APPLE)
  message(STATUS "Adding Apple definition")
  add_definitions(-DAPPLE)
endif(APPLE)

if (WIN32)
  ## visual studio
  message(STATUS "Setting MSVC flags")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHc /std:c++14")
else()
  message(status "Setting GCC flags")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -g -Wall -std=c++14")
endif(WIN32)

set(OpenCV_STATIC OFF)
set(BUILD_SHARED_LIBS ON)
find_package(OpenCV  3.0.0 REQUIRED)

SET (Boost_DEBUG TRUE)
set (Boost_NO_SYSTEM_PATHS true)
set (Boost_USE_STATIC_LIBS false)
set (Boost_USE_MULTITHREADED ON)

if (WIN32)
  message(STATUS "Setting Boost additional flags for Windows")
  # disable autolinking in boost
add_definitions( -DBOOST_ALL_NO_LIB )
  ADD_DEFINITIONS(-DBOOST_LIB_DIAGNOSTIC )
# force dynamic linking for all libraries
  add_definitions( -DBOOST_ALL_DYN_LINK )
  set(Boost_COMPILER "-vc141")
endif(WIN32)

find_package(Boost 1.64.0 COMPONENTS system program_options filesystem thread iostreams REQUIRED)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# we add the sub-directories that we want CMake to scan
if (WIN32)
  add_subdirectory(src)
  #add_subdirectory(src_win32)
else()
  add_subdirectory(src)
endif(WIN32)


