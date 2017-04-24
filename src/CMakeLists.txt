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
## @brief CMake build script for the ezCU C library.
##
project (ezcu CXX)
cmake_minimum_required (VERSION 2.8)

## Build the ezCU C static library.
include_directories(${CMAKE_SOURCE_DIR}/include)
file(GLOB C_SRCS "*.c")
cuda_add_library(ezcu STATIC ${C_SRCS})
add_dependencies(ezcu liburb_tree)
set_target_properties(ezcu PROPERTIES OUTPUT_NAME "ezCU")
install(TARGETS ezcu ARCHIVE DESTINATION lib)

## Add the pathes to ezcu which we need during post install.
get_target_property(EZCU_BUILD_LIB ezcu LOCATION)
get_filename_component(EZCU_BUILD_DIR ${EZCU_BUILD_LIB} DIRECTORY)
string(REPLACE ${EZCU_BUILD_DIR} 
       ${CMAKE_SOURCE_DIR}/lib EZCU_INSTALL_LIB 
       ${EZCU_BUILD_LIB})

install(CODE "set(EZCU_BUILD_LIB \"${EZCU_BUILD_LIB}\")")
install(CODE "set(EZCU_INSTALL_LIB \"${EZCU_INSTALL_LIB}\")")
