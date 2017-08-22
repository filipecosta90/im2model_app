file(GLOB im2model_core_src
  application_log.cpp
  application_log.hpp
  atom.hpp
  chem_database.hpp
  mc_scanner.hpp
  string_additions.hpp
  unit_cell.hpp
  atom_info.hpp
  cv_polygon.hpp
  cv_polygon.cpp
  msa_prm.hpp
  super_cell.hpp
  wavimg_prm.hpp
  celslc_prm.hpp
  simgrid.hpp
  symbcalc.hpp
  td_map.hpp
  global_def.hpp
  base_bin.hpp
  base_crystal.hpp
  base_image.hpp
  base_logger.hpp
  base_cell.hpp
  atom.cpp
  chem_database.cpp
  simgrid.cpp
  symbcalc.cpp
  atom_info.cpp
  msa_prm.cpp
  string_additions.cpp
  unit_cell.cpp
  celslc_prm.cpp
  super_cell.cpp
  wavimg_prm.cpp
  td_map.cpp
  base_bin.cpp
  base_crystal.cpp
  base_image.cpp
  base_logger.cpp
  base_cell.cpp
  image_bounds.cpp
  image_bounds.hpp
  ## FEI EMD
  emd_object.h
  emd_object.cpp
  emd_attribute.h
  emd_attribute.cpp
  emd_dataset.h
  emd_dataset.cpp
  emd_group.h
  emd_group.cpp
  emd_wrapper.h
  emd_wrapper.cpp
  )


BISON_TARGET(mc_parser mc_parser.yy ${CMAKE_CURRENT_BINARY_DIR}/mc_parser.tab.cc )

FLEX_TARGET(mc_lexer mc_lexer.l ${CMAKE_CURRENT_BINARY_DIR}/mc_lexer.yy.cc)

ADD_FLEX_BISON_DEPENDENCY(mc_lexer mc_parser)
if(WIN32)
  message(STATUS "Including Flex FlexLexer.h dir - ${FLEX_INCLUDE_DIRS}")
  include_directories ( ${FLEX_INCLUDE_DIRS} )
endif(WIN32)

# Properties->C/C++->General->Additional Include Directories
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

add_library(rapidjson INTERFACE)
#target_sources(rapidjson INTERFACE lib2.h)

target_include_directories(rapidjson INTERFACE
    "${PROJECT_SOURCE_DIR}/include/rapidjson"
)

#file(GLOB SRCS *.cpp *.hpp *.h)

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

add_executable( ${PROJECT_NAME} im2model.cpp ${im2model_core_src} mc_driver.cpp ${FLEX_mc_lexer_OUTPUTS} ${BISON_mc_parser_OUTPUTS} ${DR_PROBE_RESOURCES} )

else(build_cli)
    message(STATUS " GUI enabled. building GUI version."  )
    # Add folder where are supportive functions
    set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )
    message(STATUS "module path ${CMAKE_MODULE_PATH} " )

    if (WIN32)
      set (CMAKE_PREFIX_PATH "W:/ExtraSpaceInstall/Qt/5.9/msvc2017_64//")
      Set (QT_MAKE_EXECUTABLE "W:/ExtraSpaceInstall/Qt/5.9/msvc2017_64/bin/qmake.exe")
      Set (QT_QPA_PLATFORM_PLUGIN_PATH "W:/ExtraSpaceInstall/Qt/5.9/msvc2017_64/plugins/platforms")
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


    # As moc files are generated in the binary dir, tell CMake
    # to always look for includes there:
    set(CMAKE_INCLUDE_CURRENT_DIR ON)

    # Find the QtWidgets library
    message(STATUS "Building the Qt5 version")
    find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets 3DCore 3DExtras 3DRender 3DInput 3DLogic 3DQuick 3DQuickRender Qml QuickWidgets )

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
      QtAwesome.cpp
      QtAwesome.h
      QtAwesomeAnim.cpp
      QtAwesomeAnim.h
      qt_scene_supercell.cpp
      qt_scene_supercell.h
      group_options.h
      group_options.cpp
      renderarea.h
      renderarea.cpp
      settings.h
      settings.cpp
      configwin.h
      configwin.cpp
      treeitem.h
      treeitem.cpp
      treemodel.h
      treemodel.cpp
      treeitem_file_delegate.hpp
      treeitem_file_delegate.cpp
      cv_image_widget.h
      cv_image_widget.cpp
      cv_image_table_frame.h
      cv_image_frame.h
      cv_image_frame.cpp
      cv_image_cell_widget.h
      cv_image_delegate.h
      cv_image_delegate.cpp
      cv_tdmap_cell_image_frame.h
      cv_tdmap_cell_image_frame.cpp
      cv_tdmap_cell_image_frame_delegate.h
      cv_tdmap_cell_image_frame_delegate.cpp
      gui_tdmap_cell.h
      gui_tdmap_cell.cpp
      gui_tdmap_table.h
      gui_tdmap_table.cpp
      gui_sim_updater.hpp
      gui_stream.hpp
      qt_file_push_button.h
      qt_file_push_button.cpp
      qint_validator.h
      qdouble_validator.h
      qint_validator.cpp
      qdouble_validator.cpp
      qline_edit_tooltip.cpp
      qline_edit_tooltip.h
      gui_sim_updater.hpp
      gui_stream.hpp
      custom_tool_button.h
      custom_tool_button.cpp
      vis/components/editorutils.h
      vis/components/editorutils.cpp
      vis/components/qdummyobjectpicker.h
      vis/components/qdummyobjectpicker.cpp
      vis/inputcontrols/qtrackballcameracontroller.cpp
      vis/inputcontrols/qtrackballcameracontroller.h
      vis/inputcontrols/qtrackballcameracontroller_p.cpp
      vis/inputcontrols/qtrackballcameracontroller_p.h
      vis/unit_cell_view_widget.cpp
      vis/unit_cell_view_widget.h
      )

    # Set PROJECT_VERSION_PATCH and PROJECT_VERSION_TWEAK to 0 if not present, needed by add_project_meta
    fix_project_version()

    # Set additional project information
    set(COMPANY "uminho")
    set(COPYRIGHT "Copyright (c) 2017 Dr. Alberto José Proença, Dr. Daniel G. Stroppa, Filipe Oliveira. All rights reserved.")
    set(IDENTIFIER "com.uminho.Im2Model")

    add_project_meta(META_FILES_TO_INCLUDE)

    set(RESOURCE_FILES ../im2model.qrc)

    message("META FILES TO INCLUDE: ${META_FILES_TO_INCLUDE}")
    message(" OS BUNDLE: ${OS_BUNDLE}")

    add_executable( ${PROJECT_NAME}
      ${OS_BUNDLE}
      im2model_gui.cpp
      ${GUI_SOURCE_FILES}
      ${META_FILES_TO_INCLUDE}
      ${RESOURCE_FILES}  ${im2model_core_src} mc_driver.cpp ${FLEX_mc_lexer_OUTPUTS} ${BISON_mc_parser_OUTPUTS} ${DR_PROBE_RESOURCES}
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

if(APPLE)
  add_sanitizers(${PROJECT_NAME})
  #  set_target_properties( app PROPERTIES  CXX_INCLUDE_WHAT_YOU_USE /Users/filipeoliveira/Downloads/include-what-you-use/bin/include-what-you-use )
endif(APPLE)

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
