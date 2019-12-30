add_executable(testC TestMain.c)

target_link_libraries(testC PUBLIC SVGNativeViewerLib)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  target_compile_options(testC PUBLIC -ansi -Wpedantic -Wall -Wextra)
endif()
