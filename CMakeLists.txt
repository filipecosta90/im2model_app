# the minimum version of CMake required
cmake_minimum_required(VERSION 3.6.2)

set (CMAKE_CXX_COMPILER "clang++" )
set (CMAKE_C_COMPILER "clang" )
set (CMAKE_CXX_STANDARD 14)

project( Im2Model VERSION 0.1 LANGUAGES CXX C )

### EXTERNAL ###
# CMake module to enable sanitizers for binary targets.
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/externals/sanitizers-cmake/cmake" ${CMAKE_MODULE_PATH})
###          ###
find_package(Sanitizers)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

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

option(build_gui "Build GUI version" OFF)
OPTION(BUILD_DOXYGEN_DOCS "Build docs" OFF)

if(BUILD_DOXYGEN_DOCS)
  find_package(Doxygen)
  if(NOT DOXYGEN_FOUND)
    message(FATAL_ERROR
      "Doxygen is needed to build the documentation. Please make sure it is installed.")
  endif(NOT DOXYGEN_FOUND)

  configure_file(${PROJECT_PATH}/Doxyfile.in
    ${PROJECT_BINARY_DIR}/Doxyfile @ONLY IMMEDIATE)

  # Add a custom target to run Doxygen whenever the project is built.
  # If you do NOT want the documentation to be generated EVERY time you build the project
  # then leave out the 'ALL' keyword from the command below.
  add_custom_target(doxygen_docs ALL COMMAND ${DOXYGEN_EXECUTABLE}
    ${PROJECT_BINARY_DIR}/Doxyfile
    SOURCES ${PROJECT_BINARY_DIR}/Doxyfile)
endif(BUILD_DOXYGEN_DOCS)

if(APPLE)
  message(STATUS "Adding Apple definition")
  add_definitions(-DAPPLE)
endif(APPLE)

if (WIN32)
  ## visual studio
  message(STATUS "Setting MSVC flags")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHc /W3 /MTd /std:c++14")
  # Enable multi-core builds
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

  # Suppress warnings about "non-secure" functions
  add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS)
  # Disable warning C4244: 'conversion' conversion from 'type1' to 'type2', possible loss of data
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4244")
  # Disable warning C4267: 'var' : conversion from 'size_t' to 'type', possible loss of data (64 bit build only)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4267")

  add_definitions(-DNOMINMAX -DWIN32_LEAN_AND_MEAN -D_WINSOCK_DEPRECATED_NO_WARNINGS)


else()
  message(status "Setting GCC flags")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -fexceptions -g ") #"-fno-omit-frame-pointer -fsanitize=address -fsanitize=undefined -fsanitize=integer")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14")
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

find_package(Boost 1.64.0 COMPONENTS log log_setup system program_options filesystem thread iostreams REQUIRED)

find_package(BISON 3.0 REQUIRED)
find_package(FLEX REQUIRED)

find_package(HDF5 COMPONENTS C CXX HL REQUIRED)

#find_package( RapidJSON REQUIRED )

# we add the sub-directories that we want CMake to scan
add_subdirectory(src)

#add_subdirectory(external/emdlib)
