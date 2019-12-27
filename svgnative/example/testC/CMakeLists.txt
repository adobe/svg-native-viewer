add_executable(testC TestMain.c)

target_link_libraries(testC PUBLIC SVGNativeViewerLib)
target_compile_options(testC PUBLIC -ansi -Wpedantic -Wall -Wextra)
