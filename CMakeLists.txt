# the minimum version of CMake required
cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 11)
project(im2model LANGUAGES CXX)

if(APPLE)
  message(STATUS "Adding Apple definition")
  add_definitions(-DAPPLE)
endif(APPLE)

if (WIN32)
  macro(get_WIN32_WINNT version)
    if (CMAKE_SYSTEM_VERSION)
      set(ver ${CMAKE_SYSTEM_VERSION})
      string(REGEX MATCH "^([0-9]+).([0-9])" ver ${ver})
      string(REGEX MATCH "^([0-9]+)" verMajor ${ver})
      # Check for Windows 10, b/c we'll need to convert to hex 'A'.
      if ("${verMajor}" MATCHES "10")
        set(verMajor "A")
        string(REGEX REPLACE "^([0-9]+)" ${verMajor} ver ${ver})
      endif ("${verMajor}" MATCHES "10")
      # Remove all remaining '.' characters.
      string(REPLACE "." "" ver ${ver})
      # Prepend each digit with a zero.
      string(REGEX REPLACE "([0-9A-Z])" "0\\1" ver ${ver})
      set(${version} "0x${ver}")
    endif(CMAKE_SYSTEM_VERSION)
  endmacro(get_WIN32_WINNT)

  get_WIN32_WINNT(ver)
  message(STATUS "Adding Windows definition")
  add_definitions(-D_WIN32_WINNT=${ver})
  ## visual studio
  message(STATUS "Setting MSVC flags")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHc /std:c++latest")
else()
  message(status "Setting GCC flags")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -g -Wall")
endif(WIN32)

include(ExternalProject)
ExternalProject_Add(
  glm
  PREFIX ${CMAKE_BINARY_DIR}/glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  LOG_DOWNLOAD ON
  )
ExternalProject_Get_Property(glm source_dir)
set(GlmIncludeDir ${source_dir}/glm)

find_package(OpenCV  3.2.0 REQUIRED)

if (WIN32)
  message(STATUS "Setting Boost flags for Windows")
SET (Boost_FIND_REQUIRED TRUE)
SET (Boost_FIND_QUIETLY TRUE)
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

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
find_package(PkgConfig REQUIRED)
#find_package(OpenMP)

pkg_search_module(GLFW REQUIRED glfw3)

include_directories( ${BOOST_INCLUDE_DIRS} )
include_directories( ${OPENGL_INCLUDE_DIRS} ) 
include_directories( ${GLUT_INCLUDE_DIRS} )
include_directories( ${GLEW_INCLUDE_DIRS} )
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( ${GLFW_INCLUDE_DIRS} )

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

#if (OPENMP_FOUND)
#  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wno-deprecated-register -std=c++0x -Wconversion")

# we add the files in the include directory to be included
# while compiling all the source code
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# we add the sub-directories that we want CMake to scan
add_subdirectory(src)

