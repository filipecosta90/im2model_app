# the minimum version of CMake required
cmake_minimum_required(VERSION 3.6.2)

set (CMAKE_CXX_COMPILER "clang++" )
set (CMAKE_CXX_STANDARD 14)

project( Im2Model )

set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
ADD_DEFINITIONS(-DOpenCV_STATIC=OFF )
ADD_DEFINITIONS(-DBUILD_SHARED_LIBS=ON )
SET (OpenCV_DEBUG TRUE)

file(GLOB_RECURSE DR_PROBE_RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/assets/drprobe_clt_bin_osx/*")
message(STATUS "Found Dr Probe Resources: ${DR_PROBE_RESOURCES}")

# Turn on the ability to create folders to organize projects (.vcproj)
# It creates "CMakePredefinedTargets" folder by default and adds CMake
# defined projects like INSTALL.vcproj and ZERO_CHECK.vcproj
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(build_gui "build_gui" OFF)

if(build_gui)
  message(STATUS "GUI enabled. building command-line im2model, and gui_im2model")
  # Add folder where are supportive functions
  set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  if (WIN32)
    set (CMAKE_PREFIX_PATH "W:/ExtraSpaceInstall/Qt/5.9/msvc2017_64//")
    Set (QT_MAKE_EXECUTABLE "D:/Qt/5.8/mingw53_32/bin/qmake.exe")
    Set (CMAKE_BUILD_TYPE "Release")
    Set (Qt5Core_DIR "W:/ExtraSpaceInstall/Qt/5.9/msvc2017_64/lib/cmake/Qt5Core")
    Set (Qt5Concurrent_DIR "W:/ExtraSpaceInstall/Qt/5.9/msvc2017_64/lib/cmake/Qt5Concurrent")
    Set (Qt5Core_DIR "W:/ExtraSpaceInstall/Qt/5.9/msvc2017_64/lib/cmake/Qt5Core")
    Set (Qt5Gui_DIR "W:/ExtraSpaceInstall/Qt/5.9/msvc2017_64/lib/cmake/Qt5Gui")
    Set (Qt5Test_DIR "W:/ExtraSpaceInstall/Qt/5.9/msvc2017_64/lib/cmake/Qt5Test")
    Set (Qt5Widgets_DIR "W:/ExtraSpaceInstall/Qt/5.9/msvc2017_64/lib/cmake/Qt5Widgets")
    Set (Qt5OpenGL_DIR "W:/ExtraSpaceInstall/Qt/5.9/msvc2017_64/lib/cmake/Qt5OpenGL")
  endif (WIN32)

  # Include Qt basic functions
  include(QtCommon)

  # Instruct CMake to run moc automatically when needed.
  #  set(CMAKE_AUTOMOC ON)
  #set(CMAKE_INCLUDE_CURRENT_DIR ON)
  # Find the QtWidgets library
  message(STATUS "Building the Qt5 version")
  find_package(Qt5Core 5.2 REQUIRED)
  find_package(Qt5Gui REQUIRED)
  find_package(Qt5Widgets REQUIRED)

else(build_gui)
  message(STATUS "GUI disabled. building CLI im2model")
endif(build_gui)

if(APPLE)
  message(STATUS "Adding Apple definition")
  add_definitions(-DAPPLE)
endif(APPLE)

if (WIN32)
  ## visual studio
  message(STATUS "Setting MSVC flags")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHc /W3 /MTd /std:c++14")
else()
  message(status "Setting GCC flags")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -g -Wall -std=c++14")
endif(WIN32)

find_package(OpenCV  3.0.0 REQUIRED)
if(WIN32)
  message(status "OpenCV lib components: ${OpenCV_LIB_COMPONENTS}")
endif(WIN32)

SET (Boost_DEBUG TRUE)
set (Boost_USE_STATIC_LIBS false)
set (Boost_USE_MULTITHREADED ON)

add_definitions( -DBOOST_ALL_NO_LIB )
ADD_DEFINITIONS(-DBOOST_LIB_DIAGNOSTIC )
# force dynamic linking for all libraries
add_definitions( -DBOOST_ALL_DYN_LINK )

if (WIN32)
  message(STATUS "Setting Boost additional flags for Windows")
  # disable autolinking in boost
  set (Boost_NO_SYSTEM_PATHS true)
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
add_subdirectory(src)


