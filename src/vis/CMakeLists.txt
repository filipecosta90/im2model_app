
file(GLOB im2model_vis_src
  "*.h"
  "*.hpp"
  "*.cpp"
  "*.c"
  )

file(COPY "shaders" DESTINATION "${CMAKE_BINARY_DIR}")

