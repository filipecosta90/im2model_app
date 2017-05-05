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
  mc_driver.hpp
  simgrid_steplength.hpp
  symbcalc.hpp
  atom.cpp
  chem_database.cpp
  mc_driver.cpp
  simgrid_steplength.cpp
  symbcalc.cpp
  atom_info.cpp
  msa_prm.cpp
  string_additions.cpp
  unit_cell.cpp
  celslc_prm.cpp
  super_cell.cpp
  wavimg_prm.cpp
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

if(WIN32)
  include_directories( ${OpenCV_INCLUDE_DIRS} )
  include_directories( ${Boost_INCLUDE_DIRS} )
  link_directories( ${Boost_LIBRARY_PATH} )

  message(STATUS " Includes - ${Boost_INCLUDE_DIRS}")
  message(STATUS " Library path  - ${Boost_LIBRARY_DIRS}") 
  message(STATUS " Libraries : ${Boost_LIBRARIES}") 
  message(status "OpenCV libs: ${OpenCV_LIBS} ")
endif(WIN32)

if(build_gui)
  # Running the meta-object compiler for QObject derived classes is similar to resources, 
  # you add QT5_WRAP_CPP in your CMake file, and just add the generated files to your source files. 
  # The input for this function is the header of your class, not the implementation — even though it’s called QT5_WRAP_CPP, 
  # you must pass the file containing the declaration; 
  # this is usually the header. 
  # Those files contain a bunch of functions which are needed for the Qt meta-object system. 
  # These are stand-alone .cpp files that you usually never have to look at.
  QT5_WRAP_CPP(tst_hdr_moc configwin.h)
  QT5_WRAP_UI(tst_form_hdr configwin.ui)

  add_library(configwin ${tst_hdr_moc} ${tst_form_hdr} )
  qt5_use_modules(configwin Widgets)
  add_executable( im2model_gui im2model_gui.cpp configwin ${im2model_core_src} mc_driver.cpp ${FLEX_mc_lexer_OUTPUTS} ${BISON_mc_parser_OUTPUTS} )
  # Use the Widgets module from Qt 5.
  qt5_use_modules( im2model_gui Core Gui Widgets )
endif(build_gui)

# Set Properties->General->Configuration Type to Application(.exe)
# Creates app.exe with the listed sources (main.cxx)
# Adds sources to the Solution Explorer
#add_executable( app main.cxx )
add_executable( im2model im2model.cpp ${im2model_core_src} mc_driver.cpp ${FLEX_mc_lexer_OUTPUTS} ${BISON_mc_parser_OUTPUTS} )

# Properties->Linker->Input->Additional Dependencies
target_link_libraries( im2model ${OpenCV_LIBS} ${Boost_LIBRARIES} )
if(build_gui)
  target_link_libraries( im2model_gui ${OpenCV_LIBS} ${Boost_LIBRARIES} )
endif(build_gui)

if(APPLE)
  message(STATUS "Adding include-what-you-use ")
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

# Adds logic to INSTALL.vcproj to copy app.exe to destination directory
install (TARGETS im2model RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/install)
if(build_gui)
  install (TARGETS im2model_gui RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/install)
endif(build_gui)


