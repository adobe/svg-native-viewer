add_executable(testSkia TestSkia.cpp)

target_include_directories(testSkia PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../../third_party/stylesheet/include")
target_include_directories(testSkia PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../../third_party/skia/include/config")
target_include_directories(testSkia PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../../third_party/skia/include/core")
target_include_directories(testSkia PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../../third_party/skia/include/effects")
target_include_directories(testSkia PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../../third_party/skia/include/encode")
target_include_directories(testSkia PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../../third_party/skia/include/gpu")
target_include_directories(testSkia PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../ports/skia")

target_link_libraries(testSkia PUBLIC SVGNativeViewerLib "${SKIA_LIBRARY_PATH}")
if(APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework Cocoa")
endif()
