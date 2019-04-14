add_executable(testSkia TestSkia.cpp)

target_include_directories(testSkia PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../../third_party/stylesheet/include")
target_include_directories(testSkia PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../../third_party/skia/include/config")
target_include_directories(testSkia PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../../third_party/skia/include/core")
target_include_directories(testSkia PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../../third_party/skia/include/effects")
target_include_directories(testSkia PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../../third_party/skia/include/encode")
target_include_directories(testSkia PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../ports/skia")

if(MSVC)
target_link_libraries(testSkia PUBLIC SVGNativeViewerLib "${CMAKE_CURRENT_SOURCE_DIR}/../../../third_party/skia/lib/skia.lib")
else()
target_link_libraries(testSkia PUBLIC SVGNativeViewerLib "${CMAKE_CURRENT_SOURCE_DIR}/../../../third_party/skia/lib/osx/libskia.a")
end()