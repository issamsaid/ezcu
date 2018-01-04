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
## FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE copyright 
## holder OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
## SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
## PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
## PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
## LIABILITY, WETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
## NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
## SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##
## @file src/CMakeLists.txt
## @author Issam SAID
## @brief CMake build script for the ezcu C library.
##
cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project (ezcu LANGUAGES C CXX CUDA)

## include the ezcu headers.
include_directories(${CMAKE_SOURCE_DIR}/include)

## gather the source files.
file(GLOB C_SRCS "*.c")

## create the library.
add_library(ezcu STATIC ${C_SRCS})

## setup the appropriate dependencies. 
add_dependencies(ezcu liburb_tree)

## rename the library.
set_target_properties(ezcu PROPERTIES OUTPUT_NAME "ezcu")
install(TARGETS ezcu ARCHIVE DESTINATION lib)

## add the paths to ezcu which we need during post install.
set(EZCU_BUILD_LIB "${CMAKE_BINARY_DIR}/src/libezcu.a")

get_filename_component(EZCU_BUILD_DIR ${EZCU_BUILD_LIB} DIRECTORY)

string(REPLACE
			 ${EZCU_BUILD_DIR} 
       ${CMAKE_SOURCE_DIR}/lib EZCU_INSTALL_LIB 
       ${EZCU_BUILD_LIB})

install(CODE "set(EZCU_BUILD_LIB \"${EZCU_BUILD_LIB}\")")
install(CODE "set(EZCU_INSTALL_LIB \"${EZCU_INSTALL_LIB}\")")
