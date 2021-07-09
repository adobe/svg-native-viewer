add_executable(simpleTest simple-test.cpp)
target_link_libraries(simpleTest SVGNativeViewerLib)
target_link_libraries(simpleTest gtest_main)
add_test(NAME simple_test COMMAND simpleTest)
