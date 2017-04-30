# Properties->C/C++->General->Additional Include Directories
include_directories( ${OpenCV_INCLUDE_DIRS} )

  message(STATUS " Includes - ${Boost_INCLUDE_DIRS}")
  message(STATUS " Library  - ${Boost_LIBRARY_DIRS}")
include_directories( ${Boost_INCLUDE_DIRS} )

link_directories(${Boost_LIBRARY_DIRS})

MESSAGE( STATUS "CMAKE_INCLUDE_PATH: " ${CMAKE_INCLUDE_PATH} )
MESSAGE( STATUS "CMAKE_LIBRARY_PATH: " ${CMAKE_LIBRARY_PATH} )

## this should include unistd.h
include_directories( ${CMAKE_INCLUDE_PATH} )
link_directories( ${CMAKE_LIBRARY_PATH} )

# Set Properties->General->Configuration Type to Application(.exe)
# Creates app.exe with the listed sources (main.cxx)
# Adds sources to the Solution Explorer
add_executable( app main.cxx )

# Properties->Linker->Input->Additional Dependencies
target_link_libraries( app ${OpenCV_LIBS} ${Boost_LIBRARIES} )

# Creates a folder "executables" and adds target 
# project (app.vcproj) under it
set_property(TARGET app PROPERTY FOLDER "executables")

# Properties->General->Output Directory
set_target_properties( app PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# Adds logic to INSTALL.vcproj to copy app.exe to destination directory
install (TARGETS app RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/_install)


