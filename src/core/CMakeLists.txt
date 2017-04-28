file(GLOB im2model_core_src 
  "*.h"
  "*.hpp"
  "*.cpp"
  "*.c"
  )

BISON_TARGET(mc_parser
  mc_parser.yy
  ${CMAKE_CURRENT_BINARY_DIR}/core/mc_parser.tab.cc
  )

FLEX_TARGET(mc_lexer
  mc_lexer.l
  ${CMAKE_CURRENT_BINARY_DIR}/core/mc_lexer.yy.cc)

ADD_FLEX_BISON_DEPENDENCY(mc_lexer mc_parser)


