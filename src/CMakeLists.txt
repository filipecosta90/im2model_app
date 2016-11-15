find_package( OpenCV REQUIRED )

file(GLOB im2model_SRC
  "*.h"
  "*.cpp"
  )

add_executable( im2model ${im2model_SRC} )
target_link_libraries( im2model ${OpenCV_LIBS} ${Boost_LIBRARIES} )

