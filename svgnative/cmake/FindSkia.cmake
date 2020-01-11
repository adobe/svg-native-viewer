# - Try to find the Skia library to be linked with,
# and prepare a few variables for linker flags.
#
# ----------------------------------------------------------------------------
#
# Copyright (C) 2020 suzuki toshiya <mpsuzuki@hiroshima-u.ac.jp>
#
# This file is licensed to you under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License. You may obtain a copy
# of the License at http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software distributed under
# the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS
# OF ANY KIND, either express or implied. See the License for the specific language
# governing permissions and limitations under the License.
#
# ----------------------------------------------------------------------------
#
# SKIA_AVAILABLE
#     a boolean that usable Skia library is found or not.
#
# SKIA_LIBRARY_PATH
#     the full-path of the Skia library to be linked.
#     to use Skia DLL, the pathname of .dll.lib file
#     instead of .dll file should be given.
#
# SKIA_LIBRARY_DIR
#     the directory which the Skia library locates.
#
# SKIA_LIBRARY_LDFLAGS
#     the flags to link Skia, to be recorded in pkg-config file.
#
# WIN32_OBJECT_TYPE
#     the flag *for MSVC* to control whether the object
#     files would be sharable or non-sharable. It is set
#     to match with the format of found or specified Skia
#     library. It is deduced from the extention of the
#     found or specified library.
#
# By default, following positions would be searched.
#
# macOS: third_party/skia/lib/osx/libskia.a
# Win:   third_party/skia/lib/skia.lib
#

set(SKIA_AVAILABLE OFF)
if (NOT DEFINED SKIA_LIBRARY_PATH)
    if (APPLE)
        set(SKIA_LIBRARY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/skia/lib/osx/libskia.a")
    elseif (MSVC)
        set(SKIA_LIBRARY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/skia/lib/skia.lib")
    else()
        message(STATUS "No usable Skia library was found, please specify -DSKIA_LIBRARY_PATH=XXX.")
    endif()
endif()

if (DEFINED SKIA_LIBRARY_PATH)
    if (EXISTS "${SKIA_LIBRARY_PATH}")
        set(SKIA_AVAILABLE ON)
        message(STATUS "Skia library at ${SKIA_LIBRARY_PATH} would be used")
    else()
        message(STATUS "No usable Skia library was found at ${SKIA_LIBRARY_PATH}")
    endif()
endif()

if (SKIA_AVAILABLE)
    get_filename_component(SKIA_LIBRARY_DIR ${SKIA_LIBRARY_PATH} DIRECTORY)
    set(SKIA_LIBRARY_LDFLAGS "-L${SKIA_LIBRARY_DIR} -lskia")
    if (MSVC AND NOT DEFINED WIN32_OBJECT_TYPE)
        string(TOLOWER ${SKIA_LIBRARY_PATH} SKIA_LIBRARY_PATH_LOW)
        if (${SKIA_LIBRARY_PATH_LOW} MATCHES "\.lib$" AND NOT ${SKIA_LIBRARY_PATH_LOW} MATCHES "\.dll\.lib$")
            message(STATUS "Skia library might be static, set object type to /MT (cannot be dynamic-linked).")
            set(WIN32_OBJECT_TYPE "/MT")
        endif()
    endif()
endif()
