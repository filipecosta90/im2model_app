file(GLOB im2model_core_src 
  "*.h"
  "*.hpp"
  "*.cpp"
  "*.c"
  )

BISON_TARGET(mc_parser mc_parser.yy ${CMAKE_CURRENT_BINARY_DIR}/mc_parser.tab.cc )

FLEX_TARGET(mc_lexer mc_lexer.l ${CMAKE_CURRENT_BINARY_DIR}/mc_lexer.yy.cc)

ADD_FLEX_BISON_DEPENDENCY(mc_lexer mc_parser)
if(WIN32)
include_directories ( {${FLEX_INCLUDE_DIRS} )
endif(WIN32)

message(STATUS " Flex Includes - ${FLEX_INCLUDE_DIRS}")
message(STATUS " Flex Includes - ${FLEX_mc_lexer_OUTPUTS}")
message(STATUS " Flex Libraries - ${FLEX_LIBRARIES}")

# Properties->C/C++->General->Additional Include Directories
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

include_directories( ${OpenCV_INCLUDE_DIRS} )

message(STATUS " Includes - ${Boost_INCLUDE_DIRS}")
message(STATUS " Library  - ${Boost_LIBRARY_DIRS}")
include_directories( ${Boost_INCLUDE_DIRS} )

link_directories(${Boost_LIBRARY_DIRS})

# Set Properties->General->Configuration Type to Application(.exe)
# Creates app.exe with the listed sources (main.cxx)
# Adds sources to the Solution Explorer
#add_executable( app main.cxx )
add_executable( app ${im2model_core_src} mc_driver.cpp ${FLEX_mc_lexer_OUTPUTS} ${BISON_mc_parser_OUTPUTS} )

# Properties->Linker->Input->Additional Dependencies
target_link_libraries( app ${OpenCV_LIBS} ${Boost_LIBRARIES} )

if(APPLE)
  message(STATUS "Adding include-what-you-use ")
  #  set_target_properties( app PROPERTIES  CXX_INCLUDE_WHAT_YOU_USE /Users/filipeoliveira/Downloads/include-what-you-use/bin/include-what-you-use )
endif(APPLE)

# Creates a folder "executables" and adds target 
# project (app.vcproj) under it
set_property(TARGET app PROPERTY FOLDER "executables")

# Properties->General->Output Directory
set_target_properties( app PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# Adds logic to INSTALL.vcproj to copy app.exe to destination directory
install (TARGETS app RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/_install)

