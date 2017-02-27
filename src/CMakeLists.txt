find_package( OpenCV  3.0.0 REQUIRED)


find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

file(GLOB im2model_SRC
  "*.h"
  "*.hpp"
  "*.cpp"
  "*.c"
  )

include_directories(${OpenCV_INCLUDE_DIRS})


BISON_TARGET(mc_parser
  mc_parser.yy
  ${CMAKE_CURRENT_BINARY_DIR}/mc_parser.tab.cc)
FLEX_TARGET(mc_lexer
  mc_lexer.l
  ${CMAKE_CURRENT_BINARY_DIR}/mc_lexer.yy.cc)
ADD_FLEX_BISON_DEPENDENCY(mc_lexer mc_parser)


add_executable( im2model ${im2model_SRC}
  mc_driver.cpp
  ${FLEX_mc_lexer_OUTPUTS}
  ${BISON_mc_parser_OUTPUTS}
  )

target_include_directories(im2model
  PRIVATE
  .
  ${CMAKE_CURRENT_BINARY_DIR})

#set_property(TARGET im2model
#PROPERTY CXX_STANDARD 14)

file(COPY "shaders" DESTINATION "${CMAKE_BINARY_DIR}")



#add_custom_target(copy-runtime-files ALL
#COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/shaders ${CMAKE_BINARY_DIR}/src/shaders
# DEPENDS ${MY_TARGET})


target_include_directories(im2model PRIVATE ${GlmIncludeDir})

target_link_libraries( im2model 
${OpenCV_LIBS} 
${Boost_LIBRARIES} 
${OPENGL_LIBRARIES} 
${GLUT_LIBRARY}  
  ${GLEW_LIBRARY}
)

