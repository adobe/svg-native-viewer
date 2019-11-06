add_executable(testCairo TestCairo.cpp)

target_link_libraries(testCairo PUBLIC SVGNativeViewerLib)
target_link_libraries(testCairo PUBLIC "${CAIRO_LIBRARIES}")
