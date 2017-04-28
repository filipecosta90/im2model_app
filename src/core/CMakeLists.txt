file(GLOB im2model_core_src 
  "*.h"
  "*.hpp"
  "*.cpp"
  "*.c"
  )

BISON_TARGET(mc_parser
  mc_parser.yy
  ${CMAKE_CURRENT_BINARY_DIR}/mc_parser.tab.cc
  )

FLEX_TARGET(mc_lexer
  mc_lexer.l
  ${CMAKE_CURRENT_BINARY_DIR}/mc_lexer.yy.cc)

ADD_FLEX_BISON_DEPENDENCY(mc_lexer mc_parser)

# Set Properties->General->Configuration Type to Application(.exe)
# Creates app.exe with the listed sources (main.cxx)
# Adds sources to the Solution Explorer
add_executable( im2model 
  ${im2model_core_src}
  mc_driver.cpp
  ${FLEX_mc_lexer_OUTPUTS}
  ${BISON_mc_parser_OUTPUTS}
  )

  # Properties->C/C++->General->Additional Include Directories
target_include_directories( im2model PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

# Properties->Linker->Input->Additional Dependencies
target_link_libraries( im2model
  ${OpenCV_LIBS} 
  ${Boost_LIBRARIES} 
  )

  # Creates a folder "executables" and adds target 
# project (app.vcproj) under it
set_property(TARGET im2model PROPERTY FOLDER "executables")

# Properties->General->Output Directory
set_target_properties(im2model PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# Adds logic to INSTALL.vcproj to copy app.exe to destination directory
install (TARGETS im2model
RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/_install)

