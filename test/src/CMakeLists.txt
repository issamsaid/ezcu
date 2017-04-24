##
## @copyright Copyright (c) 2016-, Issam SAID <said.issam@gmail.com>
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
## 3. Neither the name of the copyright holder nor the names of its contributors
##    may be used to endorse or promote products derived from this software
##    without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
## INCLUDING, BUT NOT LIMITED TO, WARRANTIES OF MERCHANTABILITY AND FITNESS
## FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE copyright holder OR
## ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
## EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
## PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
## PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
## LIABILITY, WETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
## NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
## SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##
## @file test/src/CMakeLists.txt
## @author Issam SAID
## @brief CMake build script for the ezCU C/C++ unit testing.
##
project (ezcu_test CXX C)
cmake_minimum_required (VERSION 2.8)

##
## Use the gtest framework if the user sets its path 
## or download and install it as an external project
##
if (DEFINED ENV{GTEST_DIR})
    include_directories("$ENV{GTEST_DIR}/googletest/include")
    add_library(libgtest STATIC IMPORTED GLOBAL)
    set_target_properties(libgtest PROPERTIES
        "IMPORTED_LOCATION" "$ENV{GTEST_DIR}/googletest/lib/libgtest.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
    )
else (DEFINED ENV{GTEST_DIR})
    ## Download and install googletest
    ExternalProject_Add(
        gtest
        GIT_REPOSITORY https://github.com/google/googletest.git
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
        INSTALL_COMMAND ""
        UPDATE_COMMAND  "")

    ## Get googletest source and binary directories from CMake project
    ExternalProject_Get_Property(gtest SOURCE_DIR BINARY_DIR)

    ## Create a libgtest target to be used as a dependency by test programs
    add_library(libgtest IMPORTED STATIC GLOBAL)
    add_dependencies(libgtest gtest)

    ## Set libgtest properties
    set_target_properties(libgtest PROPERTIES
        "IMPORTED_LOCATION" "${BINARY_DIR}/googlemock/gtest/libgtest.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}")

    ## Include the googletest headers
    include_directories("${SOURCE_DIR}/googletest/include")
endif (DEFINED ENV{GTEST_DIR})

## Include the ezCU headers
include_directories(${CMAKE_SOURCE_DIR}/include)

## Include additional ezCU test headers
include_directories(${CMAKE_SOURCE_DIR}/test/include)

file(GLOB C_SRCS "*.cc" "*.h")

add_definitions(-DPREFIX=\"${CMAKE_SOURCE_DIR}/test/cuda\")

## Generate the executable
add_executable(ezcu_test ${C_SRCS})

## Link the executable to the ezCU, CUDA, and urb_tree and gtest
target_link_libraries (ezcu_test ezcu)
target_link_libraries (ezcu_test ${CUDA_CUDA_LIBRARY})
target_link_libraries (ezcu_test ${NVRTC_LIBRARY})
target_link_libraries (ezcu_test liburb_tree)
target_link_libraries (ezcu_test libgtest)
add_dependencies(ezcu_test ezcu)

##
## Workaround to install this target simultaneously with the main target.
## This is due to a cmake restriction: 
##  - Installing a target with the EXCLUDE_FROM_ALL target property 
##    set to TRUE has undefined behavior. 
##  
## First: inform the build location of the target. 
get_target_property(EZCU_TEST_BUILD_EXE ezcu_test LOCATION)

## Then : inform the location where the target is to be installed. 
set (EZCU_TEST_INSTALL_DIR ${CMAKE_SOURCE_DIR}/test/bin)
