# the minimum version of CMake required

project(im2model CXX)

if(APPLE)
  add_definitions(-DAPPLE)
endif(APPLE)

cmake_minimum_required(VERSION 3.1)

include(ExternalProject)

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

set(Boost_USE_STATIC_LIBS ON) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 

find_package(PkgConfig REQUIRED)
find_package(Boost COMPONENTS program_options REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenCV  3.0.0 REQUIRED)
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)


include_directories(${BOOST_INCLUDE_DIRS})
include_directories( ${OPENGL_INCLUDE_DIRS} ) 
include_directories( ${GLUT_INCLUDE_DIRS} )
include_directories( ${GLEW_INCLUDE_DIRS} )
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${GLFW_INCLUDE_DIRS})

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-deprecated-register -std=c++0x -Wconversion")

# we add the files in the include directory to be included
# while compiling all the source code
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# we add the sub-directories that we want CMake to scan
add_subdirectory(src)

