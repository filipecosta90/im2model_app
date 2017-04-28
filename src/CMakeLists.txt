add_subdirectory(core)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_executable( im2model 
  core/${im2model_core_src}
  core/mc_driver.cpp
  ${FLEX_mc_lexer_OUTPUTS}
  ${BISON_mc_parser_OUTPUTS}
  )

#target_include_directories(im2model
#  PRIVATE
#  .
#  ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries( im2model 
  ${OpenCV_LIBS} 
  ${Boost_LIBRARIES} 
  )

