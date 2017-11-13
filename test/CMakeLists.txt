file(GLOB mytest_source_files *.cpp)

add_executable(Im2ModelTest ${mytest_source_files})

target_link_libraries(Im2ModelTest gtest gmock)

