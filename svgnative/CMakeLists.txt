#[[ 
cd build
cmake -Bbuild/win64 -H. -G "Visual Studio 15 2017 Win64"
cmake -Bbuild/win32 -H. -G "Visual Studio 15 2017"
cmake -Bbuild/mac -H. -G "Xcode"
cmake -Bbuild/linux -H.

cmake --build build/win64 --config Release
]]

cmake_minimum_required(VERSION 3.5.0)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
project(SVGNativeViewer)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8.1)
        message(FATAL_ERROR "Minimum supported GCC version: 4.8.1. Currently used version: ${CMAKE_CXX_COMPILER_VERSION}")
    endif()
endif()
message(STATUS "CMAKE_GENERATOR: ${CMAKE_GENERATOR}")
message(STATUS "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")

################################
# CMake Arguments
################################
include(CMakeDependentOption)

option(LIB_ONLY "Create a project for library only" OFF)

option(TEXT "Enable Text port for testing" ON)
option(CG "Enable CoreGraphics/Quartz2D port")
option(SKIA "Enable Skia port")
option(GDIPLUS "Enable GDI+ port")
option(STYLE "Enable deprecated CSS Styling support")
option(CAIRO "Enable Cairo port")

CMAKE_DEPENDENT_OPTION(USE_TEXT "Text port for testing" TRUE "TEXT" FALSE)
CMAKE_DEPENDENT_OPTION(USE_CG "CoreGraphics needs to be enabled and run on macOS/iOS" TRUE "APPLE;CG" FALSE)
CMAKE_DEPENDENT_OPTION(USE_SKIA "Skia" TRUE "APPLE;SKIA" FALSE)
CMAKE_DEPENDENT_OPTION(USE_GDIPLUS "GDI+ port" TRUE "MSVC;GDIPLUS" FALSE)
CMAKE_DEPENDENT_OPTION(USE_CAIRO "Cairo" TRUE "CAIRO" FALSE)

CMAKE_DEPENDENT_OPTION(USE_TEXT_EXAMPLE "Text port for testing" TRUE "NOT LIB_ONLY;TEXT" FALSE)
CMAKE_DEPENDENT_OPTION(USE_CG_EXAMPLE "CoreGraphics needs to be enabled and run on macOS/iOS" TRUE "NOT LIB_ONLY;USE_CG" FALSE)
CMAKE_DEPENDENT_OPTION(USE_SKIA_EXAMPLE "Skia" TRUE "NOT LIB_ONLY;SKIA" FALSE)
CMAKE_DEPENDENT_OPTION(USE_GDIPLUS_EXAMPLE "GDI+ example app" TRUE "NOT LIB_ONLY;GDIPLUS" FALSE)
CMAKE_DEPENDENT_OPTION(USE_CAIRO_EXAMPLE "Cairo example" TRUE "NOT LIB_ONLY;CAIRO" FALSE)

################################
# setting for Cairo 
################################
if(CAIRO)
    find_package(PkgConfig)
    include(FindCairo)
    if(NOT CAIRO_FOUND)
        set(CAIRO_INCLUDE_DIRS "/usr/include/cairo")     
        set(CAIRO_LIBRARIES "-lcairo")     
    endif()
endif()

################################
# Other Attributes
################################
if(EXISTS "${CMAKE_TOOLCHAIN_FILE}")
    # do not apply any global settings if the toolchain
    # is being configured externally
elseif(MSVC)
    #add_definitions(/FI"${CMAKE_CURRENT_SOURCE_DIR}/public/include/CloudAI/ai/ASConfig.h")
    # Visual Studio pedantic build settings
    # warning C4512: assignment operator could not be generated
    # warning C4316: object allocated on the heap may not be aligned
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /wd4512 /wd4316")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zp8")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /we4302 /we4189")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4290 /wd4335 /wd4355 /wd4814 /wd4091 /TP")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++14")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:sizedDealloc-")
elseif(APPLE)
    # GCC or Clang
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
endif()

if(STYLE)
    # Enable CSS style support. This feature is deprecated and will get removed.
    add_definitions(-DSTYLE_SUPPORT)
endif()

##############################
# Demo/Testing applications
##############################
if(USE_TEXT_EXAMPLE)
add_subdirectory(example/testText)
endif()
if(USE_CG_EXAMPLE)
add_subdirectory(example/testCocoaCG)
endif()
if (USE_SKIA_EXAMPLE)
add_subdirectory(example/testSkia)
endif()
if (USE_GDIPLUS_EXAMPLE)
add_subdirectory(example/testGDIPlus)
endif()
if (USE_CAIRO_EXAMPLE)
add_subdirectory(example/testCairo)
endif()

##############################
# CPP and Headers for Library
##############################
file(GLOB gl_public
    include/SVGDocument.h
    include/SVGRenderer.h
)
file(GLOB gl_source
    src/SVGDocumentImpl.h
    src/SVGDocumentImpl.cpp
    src/SVGDocument.cpp
    src/SVGParserArcToCurve.cpp
    src/SVGStringParser.h
    src/SVGStringParser.cpp
)

set(gl_deprecated_styling)
if(STYLE)
file(GLOB gl_deprecated_styling
    src/DeprecatedSVGDocumentImpl.cpp
)
endif()

##############################
# Rendering ports
##############################
set(text_port)
if(USE_TEXT)
file(GLOB text_port
    ports/string/StringSVGRenderer.h
    ports/string/StringSVGRenderer.cpp
)
endif()

set(cg_port)
if(USE_CG)
file(GLOB cg_port
    ports/cg/CGSVGRenderer.h
    ports/cg/CGSVGRenderer.cpp
    ../third_party/cpp-base64/base64.h
    ../third_party/cpp-base64/base64.cpp
)
endif()

set(skia_port)
if (USE_SKIA)
file(GLOB skia_port
    ports/skia/SkiaSVGRenderer.h
    ports/skia/SkiaSVGRenderer.cpp
)
endif()

set(gdiplus_port)
if (USE_GDIPLUS)
file(GLOB gdiplus_port
    ports/gdiplus/GDIPlusSVGRenderer.h
    ports/gdiplus/GDIPlusSVGRenderer.cpp
    ../third_party/cpp-base64/base64.h
    ../third_party/cpp-base64/base64.cpp
)
endif()
set(cairo_port)
if (USE_CAIRO)
file(GLOB cairo_port
    ports/cairo/CairoSVGRenderer.h
    ports/cairo/CairoSVGRenderer.cpp
    ports/cairo/CairoImageInfo.h
    ports/cairo/CairoImageInfo.c
    ../third_party/cpp-base64/base64.h
    ../third_party/cpp-base64/base64.cpp
)
endif()

##############################
# Library settings
##############################
add_library(
    SVGNativeViewerLib STATIC
    ${gl_public}
    ${gl_deprecated_styling}
    ${gl_source}
    ${text_port}
    ${cg_port}
    ${skia_port}
    ${gdiplus_port}
    ${cairo_port}
)

##############################
# Header search paths
##############################

target_include_directories(SVGNativeViewerLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../")
target_include_directories(SVGNativeViewerLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(SVGNativeViewerLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
if(STYLE)
target_include_directories(SVGNativeViewerLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/stylesheet/include")
endif()
if (USE_TEXT)
target_include_directories(SVGNativeViewerLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/ports/string")
endif()
if (USE_CG)
target_include_directories(SVGNativeViewerLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/ports/cg")
target_include_directories(SVGNativeViewerLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/cpp-base64")
endif()
if (USE_SKIA)
target_include_directories(SVGNativeViewerLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/ports/skia")
target_include_directories(SVGNativeViewerLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/skia/include/config")
target_include_directories(SVGNativeViewerLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/skia/include/core")
target_include_directories(SVGNativeViewerLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/skia/include/effects")
target_include_directories(SVGNativeViewerLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/skia/include/encode")
target_include_directories(SVGNativeViewerLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/skia/include/gpu")
endif()
if (USE_GDIPLUS)
target_include_directories(SVGNativeViewerLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/ports/gdiplus")
target_include_directories(SVGNativeViewerLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/cpp-base64")
endif()
if (USE_CAIRO)
target_include_directories(SVGNativeViewerLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/ports/cairo")
target_include_directories(SVGNativeViewerLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/cpp-base64")
target_include_directories(SVGNativeViewerLib PUBLIC "${CAIRO_INCLUDE_DIRS}")
endif()

##############################
# Library search paths
##############################

if(USE_CG)
target_link_libraries(SVGNativeViewerLib "-framework CoreGraphics")
endif()
if (USE_SKIA)
if(MSVC)
target_link_libraries(SVGNativeViewerLib "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/skia/lib/skia.lib")
else()
target_link_libraries(SVGNativeViewerLib "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/skia/lib/osx/libskia.a")
endif()
endif()
if (USE_CAIRO)
target_link_libraries(SVGNativeViewerLib "${CAIRO_LIBRARIES}")
target_link_libraries(SVGNativeViewerLib "-ljpeg")
endif()

set_target_properties(SVGNativeViewerLib PROPERTIES LINKER_LANGUAGE CXX)

set(Boost_USE_STATIC_LIBS OFF)
find_package(Boost)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Fatal error: Boost (version >= 1.55) required.")
else()
    target_include_directories(SVGNativeViewerLib PUBLIC ${Boost_INCLUDE_DIRS})
endif()
