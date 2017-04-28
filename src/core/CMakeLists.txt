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

add_executable( im2model 
  ${im2model_core_src}
  mc_driver.cpp
  ${FLEX_mc_lexer_OUTPUTS}
  ${BISON_mc_parser_OUTPUTS}
  )

target_include_directories( im2model PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

target_link_libraries( im2model
  ${OpenCV_LIBS} 
  ${Boost_LIBRARIES} 
  )

