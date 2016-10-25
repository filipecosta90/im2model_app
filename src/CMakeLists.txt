find_package( OpenCV REQUIRED )

add_executable( im2model opencv.cpp )
target_link_libraries( im2model ${OpenCV_LIBS} )

