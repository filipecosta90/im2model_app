# the minimum version of CMake required
cmake_minimum_required(VERSION 3.6.2)
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
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -g -Wall")
endif(WIN32)

set(OpenCV_STATIC OFF)
set(BUILD_SHARED_LIBS ON)
find_package(OpenCV  3.0.0 REQUIRED)

if (WIN32)
  message(STATUS "Setting Boost flags for Windows")
  SET (Boost_DEBUG TRUE)
  set (Boost_USE_MULTITHREADED TRUE)
  set(Boost_USE_STATIC_LIBS        OFF)
  set(Boost_USE_STATIC_RUNTIME     OFF)
  set(BOOST_ALL_DYN_LINK           TRUE)   # force dynamic linking for all libraries
  set(Boost_COMPILER "-vc141")
else()
  message(STATUS "Setting Boost flags for Unix")
  set(Boost_USE_STATIC_LIBS        ON)
  set (Boost_USE_MULTITHREADED TRUE)
  set(Boost_USE_STATIC_RUNTIME     OFF)
endif(WIN32)

find_package(Boost 1.64.0 COMPONENTS program_options filesystem thread REQUIRED)

#SET ( BISON_EXECUTABLE C:\ProgramData\chocolatey\lib\winflexbison3\tools\win_bison.exe )
find_package(BISON REQUIRED)
#SET ( FLEX_EXECUTABLE C:\ProgramData\chocolatey\lib\winflexbison3\tools\win_flex.exe )
find_package(FLEX REQUIRED)

# unistd.h in windows
#if (WIN32)
#FIND_PACKAGE(GNUWIN32 REQUIRED)
#endif(WIN32)

# we add the sub-directories that we want CMake to scan
if (WIN32)
#add_subdirectory(src)
add_subdirectory(src_win32)
else()
 add_subdirectory(src)
endif(WIN32)


