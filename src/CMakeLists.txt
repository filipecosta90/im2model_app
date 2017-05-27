file(GLOB im2model_core_src 
  atom.hpp
  chem_database.hpp
  mc_scanner.hpp
  string_additions.hpp
  unit_cell.hpp
  atom_info.hpp
  edge.hpp
  msa_prm.hpp
  super_cell.hpp
  wavimg_prm.hpp
  celslc_prm.hpp
  simgrid_steplength.hpp
  symbcalc.hpp
  structure.hpp
  image_crystal.hpp 
  td_map.hpp
  global_def.hpp
  atom.cpp
  chem_database.cpp
  simgrid_steplength.cpp
  symbcalc.cpp
  atom_info.cpp
  msa_prm.cpp
  string_additions.cpp
  unit_cell.cpp
  celslc_prm.cpp
  super_cell.cpp
  wavimg_prm.cpp
  structure.cpp
  image_crystal.cpp
  td_map.cpp
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

include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( ${Boost_INCLUDE_DIRS} )
link_directories( ${Boost_LIBRARY_PATH} )


if(WIN32)
  include_directories( ${OpenCV_INCLUDE_DIRS} )
  include_directories( ${Boost_INCLUDE_DIRS} )
  link_directories( ${Boost_LIBRARY_PATH} )

  message(STATUS " Includes - ${Boost_INCLUDE_DIRS}")
  message(STATUS " Library path  - ${Boost_LIBRARY_DIRS}") 
  message(STATUS " Libraries : ${Boost_LIBRARIES}") 
  message(STATUS "OpenCV libs: ${OpenCV_LIBS} ")
endif(WIN32)

if(build_gui)
  # Set PROJECT_VERSION_PATCH and PROJECT_VERSION_TWEAK to 0 if not present, needed by add_project_meta

  # Running the meta-object compiler for QObject derived classes is similar to resources, 
  # you add QT5_WRAP_CPP in your CMake file, and just add the generated files to your source files. 
  # The input for this function is the header of your class, not the implementation — even though it’s called QT5_WRAP_CPP, 
  # you must pass the file containing the declaration; 
  # this is usually the header. 
  # Those files contain a bunch of functions which are needed for the Qt meta-object system. 
  # These are stand-alone .cpp files that you usually never have to look at.

  #  QT5_WRAP_CPP(tst_hdr_moc configwin.h treemodel.h )
  #QT5_WRAP_UI(tst_form_hdr configwin.ui)

  #
  #qt5_use_modules(configwin Core Gui Widgets)
  set(GUI_SOURCE_FILES
    configwin.cpp
    im2model_gui.cpp
    treeitem.cpp
    treemodel.cpp
    treeitem_file_delegate.hpp
    treeitem_file_delegate.cpp
    cv_image_widget.h
    cv_image_frame.h
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
    qt_file_push_button.h
    qt_file_push_button.cpp
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
  
  add_executable( im2model_gui ${OS_BUNDLE}  ${GUI_SOURCE_FILES} ${META_FILES_TO_INCLUDE} ${RESOURCE_FILES}  ${im2model_core_src} mc_driver.cpp ${FLEX_mc_lexer_OUTPUTS} ${BISON_mc_parser_OUTPUTS} ${DR_PROBE_RESOURCES} )
  # Use the Widgets module from Qt 5.
  qt5_use_modules( im2model_gui Core Gui Widgets )
  SET_SOURCE_FILES_PROPERTIES(${DR_PROBE_RESOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endif(build_gui)

# Creates app.exe with the listed sources (main.cxx)
# Adds sources to the Solution Explorer
add_executable( im2model im2model.cpp ${im2model_core_src} mc_driver.cpp ${FLEX_mc_lexer_OUTPUTS} ${BISON_mc_parser_OUTPUTS} ${DR_PROBE_RESOURCES} )

# Properties->Linker->Input->Additional Dependencies
target_link_libraries( im2model ${OpenCV_LIBS} ${Boost_LIBRARIES} )
if(build_gui)
  target_link_libraries( im2model_gui ${OpenCV_LIBS} ${Boost_LIBRARIES} )
endif(build_gui)

if(APPLE)

  #  set_target_properties( app PROPERTIES  CXX_INCLUDE_WHAT_YOU_USE /Users/filipeoliveira/Downloads/include-what-you-use/bin/include-what-you-use )
endif(APPLE)

# Creates a folder "executables" and adds target 
# project (app.vcproj) under it
set_property(TARGET im2model PROPERTY FOLDER "executables")

# Properties->General->Output Directory
set_target_properties( im2model PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
if(build_gui)
  set_target_properties( im2model_gui PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
endif(build_gui)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR} )


# Adds logic to INSTALL.vcproj to copy app.exe to destination directory
#--------------------------------------------------------------------------------
# Install the QtTest application, on Apple, the bundle is at the root of the
# install tree, and on other platforms it'll go into the bin directory.
INSTALL(TARGETS im2model 
  BUNDLE DESTINATION . COMPONENT Runtime
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin COMPONENT Runtime
  )

if(build_gui)
  install (
    TARGETS im2model_gui  
    RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin
    BUNDLE DESTINATION ${PROJECT_SOURCE_DIR}/bin    
    )
endif(build_gui)


#--------------------------------------------------------------------------------
# Now the installation stuff below
#--------------------------------------------------------------------------------

IF(WIN32)
  SET(APPS "${PROJECT_SOURCE_DIR}/bin/im2model.exe")
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

