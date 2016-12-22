find_package( OpenCV  3.0.0 REQUIRED)

file(GLOB im2model_SRC
  "*.h"
  "*.cpp"
  )

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable( im2model ${im2model_SRC} )
target_link_libraries( im2model ${OpenCV_LIBS} ${Boost_LIBRARIES} )

