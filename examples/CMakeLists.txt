##
## @copyright Copyright (c) 2016-, Issam SAID <said.issam@gmail.com>.
## All rights reserved.
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions
## are met:
##
## 1. Redistributions of source code must retain the above copyright
##    notice, this list of conditions and the following disclaimer.
## 2. Redistributions in binary form must reproduce the above copyright
##    notice, this list of conditions and the following disclaimer in the
##    documentation and/or other materials provided with the distribution.
## 3. Neither the name of the copyright holder nor the names of its 
##    contributors may be used to endorse or promote products derived from this
##    software without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
## INCLUDING, BUT NOT LIMITED TO, WARRANTIES OF MERCHANTABILITY AND FITNESS
## FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
## HOLDER OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
## SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
## PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
## PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
## LIABILITY, WETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
## NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
## SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##
## @file examples/CMakeLists.txt
## @author Issam SAID
## @brief CMake build script for the ezCU unit testing.
##
project (build_examples C Fortran)
cmake_minimum_required (VERSION 2.8)

##
## Use the uparser library if the user sets its path 
## or download and install it as an external project
##
if (DEFINED ENV{UPARSER_DIR})
    include_directories("$ENV{UPARSER_DIR}/include")
    add_library(libuparser STATIC IMPORTED GLOBAL)
    set_target_properties(libuparser PROPERTIES
        "IMPORTED_LOCATION" "$ENV{UPARSER_DIR}/lib/libuparser.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}")
else (DEFINED ENV{UPARSER_DIR})
    ExternalProject_Add(
        uparser
        GIT_REPOSITORY https://github.com/issamsaid/uparser.git
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/uparser
        INSTALL_COMMAND ""
        UPDATE_COMMAND  "")

    ## Get uilst source and binary directories
    ExternalProject_Get_Property(uparser SOURCE_DIR BINARY_DIR)

    ## Create a libuparser target to be used as a endency by ezCU
    add_library(libuparser STATIC IMPORTED GLOBAL)
    add_dependencies(libuparser uparser)

    ## Set libuparser properties
    set_target_properties(libuparser PROPERTIES
        "IMPORTED_LOCATION" "${BINARY_DIR}/src/libuparser.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}")

    ## Include uparser headers.
    include_directories("${SOURCE_DIR}/include")
endif (DEFINED ENV{UPARSER_DIR})

## Build the ezCU C/C++ examples.
add_subdirectory(src)

## Build the ezCU fortran examples.
add_subdirectory(fortran_interface)

## Add a custom target that includes the build of all the targets
add_custom_target(build_examples)
add_dependencies(build_examples c_examples fortran_examples)