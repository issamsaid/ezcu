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
## @file CMakeLists.txt
## @author Issam SAID
## @brief CMake main build script for ezCU.
##
project (ezcu_distribution C CXX Fortran)
cmake_minimum_required (VERSION 2.8)

## Enable ExternalProject CMake module
include(ExternalProject)
find_package(Threads REQUIRED)

## Overwrite the install prefix
set (CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}")

## Add extra cmake modules dir
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

## Configurable options for how we want to build ezCU
option(ezcu_debug       "Build ezCU with the debug mode."              OFF)
option(ezcu_verbose     "Build ezCU with the verbose mode activated."   ON)
option(ezcu_tty         "Build ezCU with logging into  the terminal."  OFF)
option(ezcu_roundup_wrk "Build ezCU with rounding up the block sizes." OFF)
option(ezcu_mpi         "Build ezCU with MPI compatibility."           OFF)

## Set the build type (DEFAULT is Release)
if (NOT CMAKE_BUILD_TYPE)
   set (CMAKE_BUILD_TYPE Release)
endif (NOT CMAKE_BUILD_TYPE)
if (ezcu_debug)
	set (CMAKE_BUILD_TYPE Debug)
endif (ezcu_debug)
message(STATUS "... CMAKE_BUILD_TYPE    : ${CMAKE_BUILD_TYPE}")

## Set the compilation flags 
if ("${CMAKE_C_COMPILER_ID}"     MATCHES "Cray")
  	include(${CMAKE_MODULE_PATH}/cray_c.cmake)
elseif ("${CMAKE_C_COMPILER_ID}" MATCHES "GNU")
    include(${CMAKE_MODULE_PATH}/gnu_c.cmake)
elseif ("${CMAKE_C_COMPILER_ID}" MATCHES "Intel")
    include(${CMAKE_MODULE_PATH}/intel_c.cmake)
elseif ("${CMAKE_C_COMPILER_ID}" MATCHES "PGI")
    include(${CMAKE_MODULE_PATH}/pgi_c.cmake)
elseif ("${CMAKE_C_COMPILER_ID}" MATCHES "MSVC")
  	include(${CMAKE_MODULE_PATH}/msvc_c.cmake)
endif ("${CMAKE_C_COMPILER_ID}"     MATCHES "Cray")

if ("${CMAKE_Fortran_COMPILER_ID}"     MATCHES "Cray")
  	include(${CMAKE_MODULE_PATH}/cray_fortran.cmake)
elseif ("${CMAKE_Fortran_COMPILER_ID}" MATCHES "GNU")
	include(${CMAKE_MODULE_PATH}/gnu_fortran.cmake)
elseif ("${CMAKE_Fortran_COMPILER_ID}" MATCHES "Intel")
  	include(${CMAKE_MODULE_PATH}/intel_fortran.cmake)
elseif ("${CMAKE_Fortran_COMPILER_ID}" MATCHES "PGI")
    	include(${CMAKE_MODULE_PATH}/pgi_fortran.cmake)
elseif ("${CMAKE_Fortran_COMPILER_ID}" MATCHES "MSVC")
  	include(${CMAKE_MODULE_PATH}/msvc_fortran.cmake)
endif ("${CMAKE_Fortran_COMPILER_ID}"     MATCHES "Cray")

if ("${CMAKE_CXX_COMPILER_ID}"     MATCHES "Cray")
  	include(${CMAKE_MODULE_PATH}/cray_cxx.cmake)
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
	include(${CMAKE_MODULE_PATH}/gnu_cxx.cmake)
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Intel")
    include(${CMAKE_MODULE_PATH}/intel_cxx.cmake)
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "PGI")
    include(${CMAKE_MODULE_PATH}/pgi_cxx.cmake)
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
  	include(${CMAKE_MODULE_PATH}/msvc_cxx.cmake)
endif ("${CMAKE_CXX_COMPILER_ID}"     MATCHES "Cray")

if (ezcu_verbose)
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -D__EZCU_VERBOSE")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__EZCU_VERBOSE")
endif (ezcu_verbose)
if (ezcu_tty)
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -D__EZCU_LOG_STD")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__EZCU_LOG_STD")
endif (ezcu_tty)
if (ezcu_roundup_wrk)
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -D__EZCU_ROUNDUP_WRK")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__EZCU_ROUNDUP_WRK")
endif (ezcu_roundup_wrk)
if (ezcu_mpi)
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -D__EZCU_MPI")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__EZCU_MPI")
endif (ezcu_mpi)

## Check whether CUDA is detected:
## - The CUDA library queered in usual directories
## - This behavior can be changed with env setting:
##   CUDA_INCLUDE_DIRS  : contains the CUDA headers
##   CUDA_LIBRARIES_DIR : contains the CUDA runtime libraries
##   CUDA_CUDA_LIBRARY  : contains the CUDA driver library
find_package (CUDA)

if (CUDA_FOUND)
    message(STATUS "... CUDA package found  : version ${CUDA_VERSION_STRING}")
	message(STATUS "... CUDA headers paths  : ${CUDA_INCLUDE_DIRS}")
	include_directories(${CUDA_INCLUDE_DIRS})
else (CUDA_FOUND)
	message(FATAL_ERROR 
			"... Unable to find your CUDA implementation headers:\n"
			"... Set the CUDA_INCLUDE_DIRS variable to locate the headers.\n")
endif (CUDA_FOUND)
	
if (${CUDA_LIBRARIES} EQUAL 'CUDA_LIBRARIES-NOTFOUND')
	message(FATAL_ERROR 
			"... Unable to find your CUDA runtime libraries:\n"
			"... Set the CUDA_LIBRARIES_DIRS variable to locate the library.\n"
			"... You may also add the library path to the LD_LIBRARY_PATH env.")
else (${CUDA_LIBRARIES} EQUAL 'CUDA_LIBRARIES-NOTFOUND')
    get_filename_component(CUDA_LIBRARIES_DIRS ${CUDA_LIBRARIES} DIRECTORY)
	message(STATUS "... CUDA library paths  : ${CUDA_LIBRARIES_DIRS}")
	message(STATUS "... CUDA libraries      : ${CUDA_LIBRARIES}")
	find_library(NVRTC_LIBRARY nvrtc ${CUDA_LIBRARIES_DIRS})
	message(STATUS "... NVRTC library       : ${NVRTC_LIBRARY}")
endif (${CUDA_LIBRARIES} EQUAL 'CUDA_LIBRARIES-NOTFOUND')

if (${CUDA_CUDA_LIBRARY} EQUAL 'CUDA_CUDA_LIBRARY-NOTFOUND')
    message(FATAL_ERROR 
            "... Unable to find your CUDA driver library:\n"
            "... Set the CUDA_CUDA_LIBRARY variable to locate the library.\n"
            "... You may also add the library path to the LD_LIBRARY_PATH env.")
else (${CUDA_CUDA_LIBRARY} EQUAL 'CUDA_CUDA_LIBRARY-NOTFOUND')
    message(STATUS "... CUDA driver library : ${CUDA_CUDA_LIBRARY}")
endif (${CUDA_CUDA_LIBRARY} EQUAL 'CUDA_CUDA_LIBRARY-NOTFOUND')

## Set(CUDA_LIBRARIES "${CUDA_LIBRARIES} ${CUDA_CUDA_LIBRARY}")
## important! don't want -std=c++0x etc. getting added to nvcc
set(CUDA_PROPAGATE_HOST_FLAGS OFF)

## So that we can build source files that have a separate .cu/.cuh
##   https://devtalk.nvidia.com/default/topic/\
##   543152/consistency-of-functions-pointer/
set(CUDA_SEPARABLE_COMPILATION ON)

## Set CUDA verbose compilation
set(CUDA_VERBOSE_BUILD ON)

## NVCC compilation options
LIST(APPEND CUDA_NVCC_FLAGS -Xcompiler)
LIST(APPEND CUDA_NVCC_FLAGS -lineinfo)
if (DEFINED ENV{EZCU_BUILD_OPTIONS})
    LIST(APPEND CUDA_NVCC_FLAGS "$ENV{EZCU_BUILD_OPTIONS}")
endif (DEFINED ENV{EZCU_BUILD_OPTIONS})

## Generate the ptx objects: mandatory before calling ezcu_load.
if (DEFINED ENV{EZCU_CC})
    set (EZCU_CC $ENV{EZCU_CC})
endif (DEFINED ENV{EZCU_CC})

##
## Use the urb_tree library if the user sets its path 
## or download and install it as an external project.
##
if (DEFINED ENV{URB_TREE_DIR})
    include_directories("$ENV{URB_TREE_DIR}/include")
    add_library(liburb_tree STATIC IMPORTED GLOBAL)
    set_target_properties(liburb_tree PROPERTIES
        "IMPORTED_LOCATION" "$ENV{URB_TREE_DIR}/lib/liburb_tree.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}")
else (DEFINED ENV{URB_TREE_DIR})
    ExternalProject_Add(
        urb_tree
        GIT_REPOSITORY https://github.com/issamsaid/urb_tree.git
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/urb_tree
        INSTALL_COMMAND ""
        UPDATE_COMMAND  "")

    ## Get urb_tree source and binary directories.
    ExternalProject_Get_Property(urb_tree SOURCE_DIR BINARY_DIR)

    ## Create a liburb_tree target to be used as a dependency by ezCU.
    add_library(liburb_tree IMPORTED STATIC GLOBAL)
    add_dependencies(liburb_tree urb_tree)

    ## Set liburb_tree properties.
    set_target_properties(liburb_tree PROPERTIES
        "IMPORTED_LOCATION" "${BINARY_DIR}/src/liburb_tree.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}")

    ## Include urb_tree headers.
    include_directories("${SOURCE_DIR}/include")
endif (DEFINED ENV{URB_TREE_DIR})

## Build the ezCU C/C++ library.
add_subdirectory(src)

## Build the ezCU Fortran interface.
add_subdirectory(fortran_interface EXCLUDE_FROM_ALL)

## Build the ezCU unit testing suit (develop branch only).
add_subdirectory(test EXCLUDE_FROM_ALL)

## Build the ezCU examples.
add_subdirectory(examples EXCLUDE_FROM_ALL)

## Build the ezCU documentation.
add_subdirectory(doc EXCLUDE_FROM_ALL)

##
## Workarounds for the lack of post_install steps:
##

## Add the path to urb_tree which we need during post install.
get_target_property(URB_TREE_LIB liburb_tree IMPORTED_LOCATION)
install(CODE "set(URB_TREE_LIB \"${URB_TREE_LIB}\")")

## Add the paths of the ezcu_fortran target which we need during post-install.
get_directory_property(EZCU_FORTRAN_BUILD_LIB DIRECTORY 
                       fortran_interface DEFINITION EZCU_FORTRAN_BUILD_LIB)
get_directory_property(EZCU_FORTRAN_INSTALL_DIR DIRECTORY 
                       fortran_interface DEFINITION EZCU_FORTRAN_INSTALL_DIR)
install(CODE "set(EZCU_FORTRAN_BUILD_LIB \"${EZCU_FORTRAN_BUILD_LIB}\")")
install(CODE "set(EZCU_FORTRAN_INSTALL_DIR \"${EZCU_FORTRAN_INSTALL_DIR}\")")

## Add the paths of the ezcu_test target which we need during post-install.
get_directory_property(EZCU_TEST_BUILD_EXE DIRECTORY 
                       test/src DEFINITION EZCU_TEST_BUILD_EXE)
get_directory_property(EZCU_TEST_INSTALL_DIR DIRECTORY 
                       test/src DEFINITION EZCU_TEST_INSTALL_DIR)
install(CODE "set(EZCU_TEST_BUILD_EXE \"${EZCU_TEST_BUILD_EXE}\")")
install(CODE "set(EZCU_TEST_INSTALL_DIR \"${EZCU_TEST_INSTALL_DIR}\")")

## Add the paths of ezcu_test_fortran which we need during post-install.
get_directory_property(EZCU_TEST_FORTRAN_BUILD_EXE DIRECTORY 
                       test/fortran_interface 
                       DEFINITION EZCU_TEST_FORTRAN_BUILD_EXE)
get_directory_property(EZCU_TEST_FORTRAN_INSTALL_DIR DIRECTORY 
                       test/fortran_interface 
                       DEFINITION EZCU_TEST_FORTRAN_INSTALL_DIR)
install(CODE 
        "set(EZCU_TEST_FORTRAN_BUILD_EXE \"${EZCU_TEST_FORTRAN_BUILD_EXE}\")")
install(CODE 
        "set(EZCU_TEST_FORTRAN_INSTALL_DIR \"${EZCU_TEST_FORTRAN_INSTALL_DIR}\")")

## Add the paths of the c_examples target which we need during post-install.
get_directory_property(EZCU_C_EXAMPLES_FILES DIRECTORY 
                       examples/src DEFINITION EZCU_C_EXAMPLES_FILES)
get_directory_property(EZCU_C_EXAMPLES_INSTALL_DIR DIRECTORY 
                       examples/src DEFINITION EZCU_C_EXAMPLES_INSTALL_DIR)
install(CODE 
        "set(EZCU_C_EXAMPLES_FILES \"${EZCU_C_EXAMPLES_FILES}\")")
install(CODE 
        "set(EZCU_C_EXAMPLES_INSTALL_DIR \"${EZCU_C_EXAMPLES_INSTALL_DIR}\")")

## Add the paths of the fortran_examples target which 
## we need during post-install.
get_directory_property(EZCU_FORTRAN_EXAMPLES_FILES DIRECTORY 
                       examples/fortran_interface 
                       DEFINITION EZCU_FORTRAN_EXAMPLES_FILES)
get_directory_property(EZCU_FORTRAN_EXAMPLES_INSTALL_DIR DIRECTORY 
                       examples/fortran_interface 
                       DEFINITION EZCU_FORTRAN_EXAMPLES_INSTALL_DIR)
install(CODE 
        "set(EZCU_FORTRAN_EXAMPLES_FILES \"${EZCU_FORTRAN_EXAMPLES_FILES}\")")
install(CODE 
        "set(EZCU_FORTRAN_EXAMPLES_INSTALL_DIR \"${EZCU_FORTRAN_EXAMPLES_INSTALL_DIR}\")")

## Add a subdirectory that contains the post-install scripts.
add_subdirectory(${CMAKE_SOURCE_DIR}/cmake/post_install)
