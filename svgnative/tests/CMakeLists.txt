set (SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/../src/Interval.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/Interval.h
)
add_executable(intervalTests interval-tests.cpp ${SOURCE_FILES})
target_include_directories(intervalTests PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../src")
target_link_libraries(intervalTests SVGNativeViewerLib)
target_link_libraries(intervalTests gtest_main)
add_test(NAME interval_tests COMMAND intervalTests)

set (SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/../src/Interval.cpp	
	${CMAKE_CURRENT_SOURCE_DIR}/../src/Rect.cpp
)
add_executable(rectangleTests rectangle-tests.cpp ${SOURCE_FILES})
target_include_directories(rectangleTests PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../src")
target_link_libraries(rectangleTests SVGNativeViewerLib)
target_link_libraries(rectangleTests gtest_main)
add_test(NAME rectangle_tests COMMAND rectangleTests)


# TODO: For now we just use the Skia port, but later on we should
# extend this and generalize this so that all the ports are equally
# tested. We'd need some mechanism to plug in different renderer
# implementations for different ports and do the tests for all those
# that are available
if (USE_SKIA)
    add_executable(boundsTests bounds-tests.cpp)
    target_link_libraries(boundsTests SVGNativeViewerLib)
    target_link_libraries(boundsTests gtest_main)
    add_test(NAME bounds_tests COMMAND boundsTests)

    target_include_directories(boundsTests PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/stylesheet/include")
    target_include_directories(boundsTests PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/skia/include/config")
    target_include_directories(boundsTests PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/skia/include/core")
    target_include_directories(boundsTests PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/skia/include/effects")
    target_include_directories(boundsTests PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/skia/include/encode")
    target_include_directories(boundsTests PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/skia/include/gpu")
    target_include_directories(boundsTests PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../ports/skia")
    target_link_libraries(boundsTests SVGNativeViewerLib "${SKIA_LIBRARY_PATH}")
endif()
