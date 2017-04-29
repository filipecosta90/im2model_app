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
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHc /std:c++latest")
else()
  message(status "Setting GCC flags")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -g -Wall")
endif(WIN32)

find_package(OpenCV  3.0.0 REQUIRED)

if (WIN32)
  message(STATUS "Setting Boost flags for Windows")
  SET (Boost_FIND_REQUIRED TRUE)
  SET (Boost_FIND_QUIETLY TRUE)
  SET (Boost_DEBUG TRUE)
  set (Boost_USE_MULTITHREADED TRUE)
  set(Boost_USE_STATIC_LIBS        TRUE)
  set(Boost_USE_STATIC_RUNTIME     OFF)
  set(BOOST_ALL_DYN_LINK           FALSE)   # force dynamic linking for all libraries
  set(Boost_COMPILER "-vc141")
else()
  message(STATUS "Setting Boost flags for Unix")
  set(Boost_USE_STATIC_LIBS        ON)
  set (Boost_USE_MULTITHREADED TRUE)
  set(Boost_USE_STATIC_RUNTIME     OFF)
endif(WIN32)

find_package(Boost 1.64.0 COMPONENTS program_options filesystem thread REQUIRED)
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# we add the sub-directories that we want CMake to scan
if (WIN32)
 add_subdirectory(src_win32)
else()
 add_subdirectory(src)
endif(WIN32)


