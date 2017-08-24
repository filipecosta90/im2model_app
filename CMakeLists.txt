# the minimum version of CMake required
cmake_minimum_required(VERSION 3.6.2)

set (CMAKE_CXX_COMPILER "g++" )
set (CMAKE_C_COMPILER "gcc" )
set (CMAKE_CXX_STANDARD 14)

set ( VERSION_IM2MODEL "0.2.3" )
project( Im2Model VERSION ${VERSION_IM2MODEL} LANGUAGES CXX C )

# Tell CMake to run moc when necessary:
  set(CMAKE_AUTOMOC ON)

  # Find includes in corresponding build directories
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  set(CMAKE_AUTORCC ON)


add_definitions( -DVERSION_IM2MODEL="${VERSION_IM2MODEL}" )

configure_file( src/version_config.h.in ${CMAKE_BINARY_DIR}/generated/version_config.h )
include_directories( ${CMAKE_BINARY_DIR}/generated/ )

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

option(build_gui "Build GUI version" ON)
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
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -fexceptions -g -fno-omit-frame-pointer ")#-fsanitize=address -fsanitize=undefined -fsanitize=integer")
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

set(CORE_SOURCE_FILES
  src/application_log.cpp
  src/application_log.hpp
  src/atom.hpp
  src/chem_database.hpp
  src/mc_scanner.hpp
  src/string_additions.hpp
  src/unit_cell.hpp
  src/atom_info.hpp
  src/cv_polygon.hpp
  src/cv_polygon.cpp
  src/msa_prm.hpp
  src/super_cell.hpp
  src/wavimg_prm.hpp
  src/celslc_prm.hpp
  src/simgrid.hpp
  src/symbcalc.hpp
  src/td_map.hpp
  src/global_def.hpp
  src/base_bin.hpp
  src/base_crystal.hpp
  src/base_image.hpp
  src/base_logger.hpp
  src/base_cell.hpp
  src/atom.cpp
  src/chem_database.cpp
  src/simgrid.cpp
  src/symbcalc.cpp
  src/atom_info.cpp
  src/msa_prm.cpp
  src/string_additions.cpp
  src/unit_cell.cpp
  src/celslc_prm.cpp
  src/super_cell.cpp
  src/wavimg_prm.cpp
  src/td_map.cpp
  src/base_bin.cpp
  src/base_crystal.cpp
  src/base_image.cpp
  src/base_logger.cpp
  src/base_cell.cpp
  src/image_bounds.cpp
  src/image_bounds.hpp
  ## FEI EMD
  src/emd_object.h
  src/emd_object.cpp
  src/emd_attribute.h
  src/emd_attribute.cpp
  src/emd_dataset.h
  src/emd_dataset.cpp
  src/emd_group.h
  src/emd_group.cpp
  src/emd_wrapper.h
  src/emd_wrapper.cpp
  )


BISON_TARGET(mc_parser ${PROJECT_SOURCE_DIR}/src/mc_parser.yy ${CMAKE_CURRENT_BINARY_DIR}/mc_parser.tab.cc )

FLEX_TARGET(mc_lexer ${PROJECT_SOURCE_DIR}/src/mc_lexer.l ${CMAKE_CURRENT_BINARY_DIR}/mc_lexer.yy.cc)

ADD_FLEX_BISON_DEPENDENCY(mc_lexer mc_parser)
if(WIN32)
  message(STATUS "Including Flex FlexLexer.h dir - ${FLEX_INCLUDE_DIRS}")
  include_directories ( ${FLEX_INCLUDE_DIRS} )
endif(WIN32)

# Properties->C/C++->General->Additional Include Directories
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/src" )
add_library(rapidjson INTERFACE)

target_include_directories(rapidjson INTERFACE
    "${PROJECT_SOURCE_DIR}/include/rapidjson"
)

include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( ${Boost_INCLUDE_DIRS} )
link_directories( ${Boost_LIBRARY_PATH} )

link_directories( ${HDF5_LIBRARY_DIRS} )
include_directories( ${HDF5_INCLUDE_DIRS} )
include_directories( ${RAPID_JSON_INCLUDE_DIR} )

if(WIN32)
  include_directories( ${OpenCV_INCLUDE_DIRS} )
  include_directories( ${Boost_INCLUDE_DIRS} )
  link_directories( ${Boost_LIBRARY_PATH} )

  message(STATUS " Includes - ${Boost_INCLUDE_DIRS}")
  message(STATUS " Library path  - ${Boost_LIBRARY_DIRS}")
  message(STATUS " Libraries : ${Boost_LIBRARIES}")
  message(STATUS "OpenCV libs: ${OpenCV_LIBS} ")
endif(WIN32)

if(build_cli)

add_executable( ${PROJECT_NAME} src/im2model.cpp ${CORE_SOURCE_FILES} src/mc_driver.cpp ${FLEX_mc_lexer_OUTPUTS} ${BISON_mc_parser_OUTPUTS} ${DR_PROBE_RESOURCES} )

else(build_cli)
    message(STATUS " GUI enabled. building GUI version."  )
    # Add folder where are supportive functions
    set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" )
    message(STATUS "module path ${CMAKE_MODULE_PATH} " )

    # Find the QtWidgets library
    message(STATUS "Building the Qt5 version")
    find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets 3DCore 3DExtras 3DRender 3DInput 3DLogic 3DQuick 3DQuickRender Qml QuickWidgets )

    # Include Qt basic functions
    include(QtCommon)

      set(ENABLE_SIGNAL_SPY false CACHE BOOL "if Qt private headers are installed, enable this to debug Qt signals")

    # Set PROJECT_VERSION_PATCH and PROJECT_VERSION_TWEAK to 0 if not present, needed by add_project_meta

    # Running the meta-object compiler for QObject derived classes is similar to resources,
    # you add QT5_WRAP_CPP in your CMake file, and just add the generated files to your source files.
    # The input for this function is the header of your class, not the implementation — even though it’s called QT5_WRAP_CPP,
    # you must pass the file containing the declaration;
    # this is usually the header.
    # Those files contain a bunch of functions which are needed for the Qt meta-object system.
    # These are stand-alone .cpp files that you usually never have to look at.

    set(GUI_SOURCE_FILES
      src/QtAwesome.cpp
      src/QtAwesome.h
      src/QtAwesomeAnim.cpp
      src/QtAwesomeAnim.h
      src/qt_scene_supercell.cpp
      src/qt_scene_supercell.h
      src/group_options.h
      src/group_options.cpp
      src/renderarea.h
      src/renderarea.cpp
      src/settings.h
      src/settings.cpp
      src/configwin.h
      src/configwin.cpp
      src/treeitem.h
      src/treeitem.cpp
      src/treemodel.h
      src/treemodel.cpp
      src/treeitem_file_delegate.hpp
      src/treeitem_file_delegate.cpp
      src/cv_image_widget.h
      src/cv_image_widget.cpp
      src/cv_image_table_frame.h
      src/cv_image_frame.h
      src/cv_image_frame.cpp
      src/cv_image_cell_widget.h
      src/cv_image_delegate.h
      src/cv_image_delegate.cpp
      src/cv_tdmap_cell_image_frame.h
      src/cv_tdmap_cell_image_frame.cpp
      src/cv_tdmap_cell_image_frame_delegate.h
      src/cv_tdmap_cell_image_frame_delegate.cpp
      src/gui_tdmap_cell.h
      src/gui_tdmap_cell.cpp
      src/gui_tdmap_table.h
      src/gui_tdmap_table.cpp
      src/gui_sim_updater.hpp
      src/gui_stream.hpp
      src/qt_file_push_button.h
      src/qt_file_push_button.cpp
      src/qint_validator.h
      src/qdouble_validator.h
      src/qint_validator.cpp
      src/qdouble_validator.cpp
      src/qline_edit_tooltip.cpp
      src/qline_edit_tooltip.h
      src/gui_sim_updater.hpp
      src/gui_stream.hpp
      src/custom_tool_button.h
      src/custom_tool_button.cpp
      src/vis/components/editorutils.h
      src/vis/components/editorutils.cpp
      src/vis/components/qdummyobjectpicker.h
      src/vis/components/qdummyobjectpicker.cpp
      src/vis/inputcontrols/qtrackballcameracontroller.cpp
      src/vis/inputcontrols/qtrackballcameracontroller.h
      src/vis/inputcontrols/qtrackballcameracontroller_p.cpp
      src/vis/inputcontrols/qtrackballcameracontroller_p.h
      src/vis/unit_cell_view_widget.cpp
      src/vis/unit_cell_view_widget.h
      )


      set(RESOURCE_FOLDER "${PROJECT_SOURCE_DIR}/src/res")
      set(RESOURCE_FILES im2model.qrc)

    # Set PROJECT_VERSION_PATCH and PROJECT_VERSION_TWEAK to 0 if not present, needed by add_project_meta
    fix_project_version()

    # Set additional project information
    set(COMPANY "uminho")
    set(COPYRIGHT "Copyright (c) 2017 Dr. Alberto José Proença, Dr. Daniel G. Stroppa, Filipe Oliveira. All rights reserved.")
    set(IDENTIFIER "com.uminho.Im2Model")

    add_project_meta(META_FILES_TO_INCLUDE)

    message("META FILES TO INCLUDE: ${META_FILES_TO_INCLUDE}")
    message(" OS BUNDLE: ${OS_BUNDLE}")

    # compile
    add_executable( ${PROJECT_NAME}
      ${OS_BUNDLE}
      src/im2model_gui.cpp
      ${GUI_SOURCE_FILES}
      ${META_FILES_TO_INCLUDE}
      ${RESOURCE_FILES}  ${CORE_SOURCE_FILES} src/mc_driver.cpp ${FLEX_mc_lexer_OUTPUTS} ${BISON_mc_parser_OUTPUTS} ${DR_PROBE_RESOURCES}
      )
      # Use the Widgets module from Qt 5.
    qt5_use_modules( ${PROJECT_NAME} Core Gui Widgets 3DCore 3DExtras 3DRender 3DInput 3DLogic 3DQuick QuickWidgets 3DQuickRender Qml )

  #include qt5 dirs
    foreach(module Core Gui Widgets 3DCore 3DExtras 3DRender 3DInput 3DLogic 3DQuick 3DQuickRender Qml QuickWidgets )
      set(qt_module Qt${module})
      set(qt5_module Qt5${module})
      find_package(${qt5_module})
      if("${${qt5_module}_PRIVATE_INCLUDE_DIRS}" STREQUAL "")
        foreach(base_dir ${${qt5_module}_INCLUDE_DIRS})
          if("${base_dir}" MATCHES "/${qt_module}\$")
            list(APPEND ${qt5_module}_PRIVATE_INCLUDE_DIRS "${base_dir}/${${qt5_module}_VERSION}/${qt_module}")
          endif()
        endforeach()
      endif()
      target_include_directories(${PROJECT_NAME} PRIVATE ${${qt5_module}_PRIVATE_INCLUDE_DIRS})
      message(STATUS "Including qt5 private dir - ${${qt5_module}_PRIVATE_INCLUDE_DIRS}\n")
    endforeach()

    # link the qt3d libraries
    target_link_libraries(${PROJECT_NAME}
      Qt5::Core
      Qt5::Gui
      Qt5::Widgets
      Qt5::QuickWidgets
      Qt5::3DCore
      Qt5::3DExtras
      Qt5::3DRender
      Qt5::3DInput
      Qt5::3DQuickRender
      Qt5::3DQuick
      )

    SET_SOURCE_FILES_PROPERTIES(${DR_PROBE_RESOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

    fix_win_compiler()
endif(build_cli)


# Creates app.exe with the listed sources (main.cxx)
# Adds sources to the Solution Explorer

# Properties->Linker->Input->Additional Dependencies
target_link_libraries( ${PROJECT_NAME} rapidjson ${OpenCV_LIBS} ${Boost_LIBRARIES} ${HDF5_CXX_LIBRARIES} )

# Creates a folder "executables" and adds target
# project (app.vcproj) under it
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "executables")

# Properties->General->Output Directory
set_target_properties( ${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR} )

# Adds logic to INSTALL.vcproj to copy app.exe to destination directory
#--------------------------------------------------------------------------------
# Install the QtTest application, on Apple, the bundle is at the root of the
# install tree, and on other platforms it'll go into the bin directory.
INSTALL(TARGETS ${PROJECT_NAME}
  BUNDLE DESTINATION . COMPONENT Runtime
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin COMPONENT Runtime
  )

#--------------------------------------------------------------------------------
# Now the installation stuff below
#--------------------------------------------------------------------------------

IF(WIN32)
  SET(APPS "${PROJECT_SOURCE_DIR}/bin/${PROJECT_NAME}.exe")
ENDIF(WIN32)
IF(APPLE)
ENDIF(APPLE)

#--------------------------------------------------------------------------------
# Use BundleUtilities to get all other dependencies for the application to work.
# It takes a bundle or executable along with possible plugins and inspects it
# for dependencies.  If they are not system dependencies, they are copied.

# directories to look for dependencies
SET(DIRS ${Boost_LIBRARY_PATH} ${OPENCV_LIB_DIRS} )

# Now the work of copying dependencies into the bundle/package
# The quotes are escaped and variables to use at install time have their $ escaped
# An alternative is the do a configure_file() on a script and use install(SCRIPT  ...).
# Note that the image plugins depend on QtSvg and QtXml, and it got those copied
# over.
INSTALL(CODE "
include(BundleUtilities)
fixup_bundle( \"${APPS}\" \"\" \"${DIRS}\")
" COMPONENT Runtime)

include(CPack)
